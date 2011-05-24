/*!
 *
 * Copyright (C) 2007 Technical University of Liberec.  All rights reserved.
 *
 * Please make a following refer to Flow123d on your project site if you use the program for any purpose,
 * especially for academic research:
 * Flow123d, Research Centre: Advanced Remedial Technologies, Technical University of Liberec, Czech Republic
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 021110-1307, USA.
 *
 *
 * $Id$
 * $Revision$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 * @file
 * @brief ???
 *
 */

#ifndef MAKE_NEIGHBOURS_H
#define MAKE_NEIGHBOURS_H

#include "mesh/mesh_types.hh"

class Mesh;
class Element;
struct Neighbour;
struct Side;
struct Edge;

/**
 * Navrh algoritmu pro hledani pruniku elementu dvou siti (libovlnych dimenzi)
 * algoritmus postupuje od bodu pruniku pres usecky a polygony k mnohostenum
 *
 * Vstup: Sit1 dimenze d1 a Sit2 dimenze d2
 * predpoladam d1<=d2
 *
 * 1) hladam body na hranici pruniku tj.
 *    Intersection<d> <d_e1,d_e2> .. prunik ma dimenzi d a pronikaji se simplexy dimenze d_e1, a d_e2
 *
 *    Intersection<0><0,0>  .. totozne vrcholy El<0>
 *    Intersection<0><0,1> a <1,0> .. vrchol jedne site lezi na hrane druhe site
 *    Intersection<0><0,n> a <n,0> .. vrchol lezi na El<n> druhe site
 *
 *    Intersection<0><1,1> .. bodovy prusecik dvou usecek v rovine
 *    Intersection<0><1,2> a <2,1> ... prusecik hrany a trojuhelnika
 *    ... dalsi zvlastni pripady vcetne <0><3,3> .. tetrahedrony s vrcholem na povrchu druheho
 *
 * 2) liniove pruniky Intersection<1>:
 *    Intersection<1><1,1> .. usecky na spolecne primce
 *    Intersection<1><1,2> a <2,1>.. usecka v rovine trojuhelnika
 *    Intersection<1><1,3> a <3,1> .. usecka a tetrahedron
 *    Intersection<1><2,2> .. prusecik dvou trojuhelniku
 *    Intersection<1><2,3> a <3,2> .. trojuhelnik a hrana tetrahedronu
 *    ..
 *
 * ... doprcic je to fakt hodne moznosti a je otazka, zda je nutne je vsechny rozlisovat
 *
 * Algoritmus by mel probuhat takto:
 *
 * 1) Najdu vrchol V site 1 a element E site 2 aby V byl v E
 *    (to neni tak trivialni, pokud site nepokryvaji stejnou oblast ale snad by to slo hledat v
 *     pruniku obalovych boxu)
 * 2) najdu pruseciky P_i hran z vrcholu V s povrchem E,
 *    konstruuju vsechny potrebne pruniky elementu majici vrchol V s elementem E
 *
 *    Sousedni elementy spolu s hranami ktere do nich vedou ulozim do prioritni fronty.
 *
 * 3) Vyberu z prioritni fronty novy E, pricemz vyuzivam spositane pruseciky psislusne steny a okoli vrcholu V
 *    tj. jdu po hranach po kterych jsem do noveho lementu prisel a najdu vsechny hranove pruniky, pak konstuuju slozitejsi pruniky
 *    az mam vsechny pruniky s novym elementem ...
 *
 *    ...
 *
 *    Prioritni fronta by preferovala elementy do kterych jsem se nejvicekrat dostal, tim se snazim minimalizovat povrch projite oblasti.
 *    Je ale mozne, ze to algoritmus naopak zpomali, pokud je prioritni fronta log(n).
 *
 *    Zpracovani jednoho elementu tedy zahrnuje
 *    1) trasovani hran:
 *       pro hranu H: testuju hledam prusecik se ctyrstenem:
 *         ANO -> pamatuju si hranovy prunik a ke stene (resp. sousednimu elementu) kde hrana vychazi pridam vychozi hranu
 *         NE -> konci ve vrcholu, dalsi hrany vychazejici z vrcholu pridam na seznam hran vchazejicich do elementu
 *
 *    2) po nalezeni pruniku vsech hran, hledam pruniky vsech vchazejicich ploch:
 *       jedna plocha ma se vstupni stenou useckovy prunik na jehoz konci jsou:
 *        *vstupni hrana
 *        * okraj steny
 *       kazdopadne trasuju okraj plosneho pruniku pres povrch elementu, nebo po vstupnich hranach,
 *       pokud prunikova plocha obsahuje vrchol tvorim nove vstupni plochy ...
 *
 *    3) Podobne trasuju vchazejici objemy
 *
 *    ?? lze nejak vyuzit pokud ma element vice vstupnich sten
 *       minimalne se da kontrolovat ...
 *
 *
 *  Struktura systemu pruniku do budoucna:
 *  1) trida IntersectionManager, ma matici vektoru. Na poli A(i,j) je vektor lokalnich souradnic na elementu dimenze i
 *     pruniku dimenze j.
 *  2) Jeden intersection objekt je pak iterator dvou elementu a dva indexy lokalnich souradnic v prislusnych vektorech
 *
 *  Prozatim to zjednodusime tak, ze
 *
 *
 *  Nakonec potrebuju pocitat integral pres prunik z nejake funkce f(phi_a(x), phi_b(x)), kde phi_a je bazova funkce na jednom elementu a phi_b na druhem.
 *  To budu delat numerickou kvadraturou, takze potrebuji zobrazit prunik na jednotkovy simplex. Pro uzel kvardatury x_i musim najit body a_i a b_i na
 *  referencnich elementech A a B. Tj potrebuju lokalni souradnice (to jsou souradnice na referencnich elementech) kvadraturnich bodu. V nic pak umim spocitat hodnotu bazovych funkci
 *  a pak i hodnotu funkce f.
 *
 *
 *
 */



//=============================================================================
// STRUCTURE OF THE NEIGHBOUR
//=============================================================================
typedef struct Neighbour
{
    int   id;           // Global id number
    int   type;         // Type
    int   n_sides;      // # of neighbouring sides
    int   n_elements;   // # of neighbouring elements
    //int   n_coefs;      // # of coefficients
    int  *sid;      // id numbers of neighbouring sides
    int  *eid;      // id numbers of neighbouring elements
    double flux;           // flux for VV neigh. from e1 into e2 is negative
                                // from e2 into e1 is positive
    //double *coef;       // coefficients of neighbouring
    double  sigma;      // transition coefficient
    double geom_factor; // fraction of the lower dimension element for VV ngh
    struct Edge *edge;  // edge (set of neighbouring sides)
    struct Side **side; // neighbouring sides
    ElementIter *element;  // neighbouring elements
                               // for VB  - element[0] is element of lower dimension
    char *line;     // Type specific part of line of the input file
    // List
    struct Neighbour *prev; // Previous neighbour in the list
    struct Neighbour *next; // Next neighbour in the list
    // Misc
    int  aux;       // Auxiliary flag
    double   faux;      // Auxiliary number
} Neighbour;

// Input neigbouring codes
#define BB_E         10     // two elements of same dim specified by eid
#define BB_EL        11     // two elements ... specified by explicit eid and sid
#define VB_ES        20     // compatible
#define VV_2E        30     // noncompatible

#define FOR_NEIGHBOURS(i)   for((i)=mesh->neighbour;(i)!=NULL;(i)=(i)->next)
#define FOR_NEIGH_ELEMENTS(i,j) for((j)=0;(j)<(i)->n_elements;(j)++)
#define FOR_NEIGH_SIDES(i,j)    for((j)=0;(j)<(i)->n_sides;(j)++)

void read_neighbour_list(Mesh*);

#endif
//-----------------------------------------------------------------------------
// vim: set cindent:
