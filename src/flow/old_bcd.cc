/*
 * old_bcd.cc
 *
 *  Created on: Jan 28, 2013
 *      Author: jb
 */


#include "flow/old_bcd.hh"
#include "fields/field_elementwise.hh"
#include "mesh/region.hh"

#include "system/tokenizer.hh"
#include "boost/lexical_cast.hpp"
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

OldBcdInput * OldBcdInput::instance() {
    static OldBcdInput *obcd = new OldBcdInput;
    return obcd;
}

// set all regions of the given Field<...> @p target
template <int spacedim, class Value>
void OldBcdInput::set_all( Field<spacedim,Value> &target, Mesh *mesh) {
    boost::shared_ptr< FieldElementwise<spacedim, Value> > in_field
        = boost::make_shared< FieldElementwise<spacedim, Value> >(target.n_comp());

    target.set_field( mesh->region_db().get_region_set("BOUNDARY"), in_field);
    target.set_mesh(mesh);

}

template <int spacedim, class Value>
void OldBcdInput::set_field( Field<spacedim,Value> &target, unsigned int bcd_ele_idx, typename Value::return_type &val) {
    boost::static_pointer_cast< FieldElementwise<spacedim, Value> >(
            target[ some_bc_region_ ]
            )->set_data_row(bcd_ele_idx, val);
}


#define DIRICHLET   1
#define NEUMANN     2
#define NEWTON      3

void OldBcdInput::read_flow(const FilePath &flow_bcd,
        Field<3,FieldValue<3>::Enum > &flow_type,
        Field<3,FieldValue<3>::Scalar > &flow_pressure,
        Field<3,FieldValue<3>::Scalar > &flow_flux,
        Field<3,FieldValue<3>::Scalar > &flow_sigma)
{
    using namespace boost;

    vector< unsigned int *> old_to_new_side_numbering;

    // in the file the sides are numbered according to opposite nodes as they appear in the MSH file
    unsigned int sides_0 [1] = {0};
    old_to_new_side_numbering.push_back( sides_0 );
    unsigned int sides_1 [2] = {0,1};
    old_to_new_side_numbering.push_back(  sides_1 );
    unsigned int sides_2 [3] = {0,1,2}; //{0,2,1};
    old_to_new_side_numbering.push_back(  sides_2 );
    unsigned int sides_3 [4] = {0,1,2,3}; //{3,2,1,0};
    old_to_new_side_numbering.push_back(  sides_3 );

    // check that all fields has same mesh, reuse it for reader
    mesh_=flow_type.mesh();
    ASSERT(mesh_ , "Null mesh pointer.\n");
    ASSERT(mesh_==flow_pressure.mesh(), "Fields initialized by OldBcdInput has different meshes (flow_pressure).\n");
    ASSERT(mesh_==flow_flux.mesh(), "Fields initialized by OldBcdInput has different meshes (flow_flux).\n");
    ASSERT(mesh_==flow_sigma.mesh(), "Fields initialized by OldBcdInput has different meshes (flow_sigma).\n");
/*
 * - read one flow file, fill fields, make ID list
 * - read second file, check IDs agains ID list, fill fields
 */
    set_all(flow_type, mesh_);
    set_all(flow_pressure, mesh_);
    set_all(flow_flux, mesh_);
    set_all(flow_sigma, mesh_);


    Tokenizer tok(flow_bcd);
    try {
        double scalar, flux, sigma;
        unsigned int id;

        xprintf(Msg, "Reading old BCD file for flow: %s ...", tok.f_name().c_str());
        tok.skip_to("$BoundaryConditions");
        tok.next_line(false);
        unsigned int n_boundaries = lexical_cast<unsigned int>(*tok); ++tok;

        for(unsigned int i_bcd=0; i_bcd < n_boundaries; i_bcd++) {
            tok.next_line();

            id = lexical_cast<unsigned int>(*tok); ++tok;

            unsigned int type  = lexical_cast<unsigned int>(*tok); ++tok;

            switch( type ) {
                case DIRICHLET:
                    scalar = lexical_cast<double>(*tok); ++tok;
                    flux = 0.0;
                    sigma = 0.0;
                    break;
                case NEUMANN:
                    flux   = lexical_cast<double>(*tok); ++tok;
                    sigma = 0.0;
                    scalar = 0.0;
                    break;
                case NEWTON:
                    scalar = lexical_cast<double>(*tok); ++tok;
                    sigma  = lexical_cast<double>(*tok); ++tok;
                    flux = 0.0;
                    break;
                default :
                    xprintf(UsrErr,"Unknown type of boundary condition - cond # %d, type %c\n", id, type );
                    break;
            }

            unsigned int where  = lexical_cast<unsigned int>(*tok); ++tok;

            unsigned int eid, sid, bc_ele_idx, our_sid;
            ElementIter ele;
            Boundary * bcd;

            switch( where ) {
                case 2: // SIDE_EL - BC given by element and its local side number
                    eid = lexical_cast<unsigned int>(*tok); ++tok;
                    sid = lexical_cast<unsigned int>(*tok); ++tok;

                    // find and set the side
                    ele = mesh_->element.find_id( eid );
                    if( sid < 0 || sid >= ele->n_sides() )
                         xprintf(UsrErr,"Boundary %d has incorrect reference to side %d\n", id, sid );
                    our_sid=old_to_new_side_numbering[ele->dim()][sid];
                    bcd = ele->side(our_sid) -> cond();
                    if (! bcd)
                        xprintf(UsrErr, "Setting boundary condition %d for non-boundary side %d of element ID: %d\n", id, sid, eid);
                    bc_ele_idx = mesh_->bc_elements.index( ele->side(our_sid) -> cond()->element() );
                    id_2_bcd_[id]= bc_ele_idx;
                    if ( ! some_bc_region_.is_valid() ) some_bc_region_ = ele->side(our_sid) -> cond()->element()->region();

                    set_field(flow_type,     bc_ele_idx, type);
                    set_field(flow_pressure, bc_ele_idx, scalar);
                    set_field(flow_flux,     bc_ele_idx, flux);
                    set_field(flow_sigma,    bc_ele_idx, sigma);
                    break;
                case 3: // SIDE_E - BC given only by element, apply to all its sides

                    xprintf(UsrErr, "Element only BCD are not supported.\n");
                    /*
                    eid = atoi( xstrtok( NULL) );

                    // find and set all exterior sides, possibly add more boundaries
                    ele = mesh->element.find_id( eid );
                    n_exterior=0;
                    FOR_ELEMENT_SIDES(ele, li) {
                        sde = ele->side( li );
                        if ( bcd=sde->cond() ) {

                            if (n_exterior > 0) {
                                xprintf(UsrErr, "Implicitly setting BC %d on more then one exterior sides of the element %d.\n", bcd_id, eid);
                                //BoundaryFullIter new_bcd = mesh->boundary.add_item();
                                // *new_bcd = *bcd;
                                //bcd=new_bcd;
                            }
                            bcd->type = type;
                            bcd->flux = flux;
                            bcd->scalar = scalar;
                            bcd->sigma = sigma;
                            n_exterior++;
                        }
                    }
                    */
                    break;
                default:
                    xprintf(UsrErr,"Unknown entity for boundary condition - cond # %d, ent. %c\n", id, where );
                    break;
            }
            unsigned int n_tags  = lexical_cast<unsigned int>(*tok); ++tok;
            while (n_tags>0) ++tok, --n_tags; // skip remaining tags



            // There was possibility to set group IDs for boundary faces (like regions for boundary elements)
            // It is probably not used so we do not implement it as it is DEPRECATED
            /*
            n_tags  = atoi( xstrtok( NULL) );
            if( n_tags > 0 ) {
                int group_id = atoi( xstrtok( NULL) );
                flow::VectorId<int>::FullIter group_iter( mesh->bcd_group_id.find_id(group_id) );

                if ( group_iter == mesh->bcd_group_id.end() ) {
                    // not found -> create new group
                    group_iter = mesh->bcd_group_id.add_item(group_id);
                }
                bcd->group = group_iter.index();   // in fact we do not use integres stored in the vector, but we use index
            }
            */
        }
        xprintf(Msg, "DONE\n");
    } catch (bad_lexical_cast &) {
        xprintf(UsrErr, "Wrong format of number, %s.\n", tok.position_msg().c_str());
    } // flow bcd reader
}



void OldBcdInput::read_transport(const FilePath &transport_bcd,
            Field<3,FieldValue<3>::Vector > &trans_conc)
{
    using namespace boost;

    set_all(trans_conc, mesh_);
    unsigned int n_substances = trans_conc.n_comp();
    FieldValue<3>::Vector::return_type ele_value(n_substances);

    Tokenizer tok(transport_bcd);
    try {
        unsigned int bcd_id, boundary_id, bc_ele_idx;

        xprintf(Msg, "Reading old BCD file for transport: %s ...", tok.f_name().c_str());
        if (tok.skip_to("$Transport_BCDFormat")) tok.next_line(false);
        tok.skip_to("$Transport_BCD");
        tok.next_line(false);
        unsigned int n_bcd = lexical_cast<unsigned int>(*tok); ++tok;
        for (unsigned int i_bcd = 0; i_bcd < n_bcd; i_bcd++) {
            tok.next_line();
            bcd_id = lexical_cast<unsigned int>(*tok); ++tok;
            boundary_id = lexical_cast<unsigned int>(*tok); ++tok;

            map<unsigned int, unsigned int>::const_iterator it = id_2_bcd_.find(bcd_id);
            if (it == id_2_bcd_.end())
                xprintf(UsrErr,"Wrong boundary index %d for bcd id %d in transport bcd file!", boundary_id, bcd_id);
            bc_ele_idx = it->second;

            for (unsigned int sbi = 0; sbi < n_substances; sbi++) {
                ele_value[sbi] = lexical_cast<double>(*tok); ++tok;
            }

            set_field(trans_conc,     bc_ele_idx, ele_value);

        }

        xprintf(Msg, "DONE\n");

    } catch (bad_lexical_cast &) {
        xprintf(UsrErr, "Wrong format of number, %s.\n", tok.position_msg().c_str());
    } // flow bcd reader
}
