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
 * @brief
 *
 * @author Jan Brezina
 */

#include "system/system.hh"
#include "time_marks.hh"
#include "time_governor.hh"
#include <algorithm>
#include <limits>

// ------------------------------------------------------
// implementation of members of class TimeMark
// ------------------------------------------------------

ostream& operator<<(ostream& stream, const TimeMark &mark)
{
    return ( stream << mark.time()<<": 0o" << hex << mark.mark_type() << dec );
}

//const TimeMark::Type TimeMark::strict =  0x1;
const TimeMark::Type TimeMark::every_type =  ~0x0;




// ------------------------------------------------------
// implementation of members of class TimeMarks
// ------------------------------------------------------

TimeMarks::TimeMarks()
: next_mark_type_(0x1)
{
    // add predefined base mark types
    type_fixed_time_ = new_mark_type();
    type_output_ = new_mark_type();
    type_bc_change_ = new_mark_type();
    type_checkpointing_ = new_mark_type();
    xprintf(Msg, "type_fixed_time_:%i, type_output_:%i, type_bc_change_:%i, type_checkpointing_:%i\n",type_fixed_time_, type_output_, type_bc_change_, type_checkpointing_);

    // insert start and end stoppers
    marks_.push_back(TimeMark(-INFINITY, TimeMark::every_type));
    marks_.push_back(TimeMark(+INFINITY, TimeMark::every_type));
}


TimeMark::Type TimeMarks::new_mark_type() {
    xprintf(Msg, "new_mark_type\n");
    ASSERT(next_mark_type_ != 0, "Can not allocate new mark type. The limit is 32 mark types.\n");
    TimeMark::Type current_type = next_mark_type_;

    next_mark_type_ <<= 1;
    return current_type;
}

//TimeMark::Type TimeMarks::new_strict_mark_type() {
//    return new_mark_type() | TimeMark::strict;
//}

void TimeMarks::add(const TimeMark &mark) {
    // find first mark with time greater or equal to the new mark
    vector<TimeMark>::iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), mark);

    // check equivalence with found mark
    if (fabs(first_ge->time() - mark.time()) < 2*numeric_limits<double>::epsilon()) {
        first_ge->add_to_type(mark.mark_type());
        return;
    }
    // possibly check equivalence with previous mark
    if (first_ge != marks_.begin()) {
        vector<TimeMark>::iterator previous = first_ge;
        --previous;
        if (fabs(previous->time() - mark.time()) < 2*numeric_limits<double>::epsilon()) {
            previous->add_to_type(mark.mark_type());
            return;
        }
    }

    marks_.insert(first_ge, mark);
}

void TimeMarks::add_time_marks(double time, double dt, double end_time, TimeMark::Type type) {
    if (end_time == TimeGovernor::inf_time) {
        if (time == TimeGovernor::inf_time) return;
        else add(TimeMark(time, type));
    }
    else {
        for (double t = time; t <= end_time*1.001; t += dt) {
            add(TimeMark(t, type));
        }
    }
}

bool TimeMarks::is_current(const TimeGovernor &tg, const TimeMark::Type &mask) const
{
    if (tg.end_time() == TimeGovernor::inf_time) return (tg.t() == TimeGovernor::inf_time);
    const TimeMark &tm = *last(tg, mask);
    //cout << "last: " << tm << endl;
    return tg.lt(tm.time() + tg.dt()); // last_t + dt < mark_t + dt
}

TimeMarks::iterator TimeMarks::next(const TimeGovernor &tg, const TimeMark::Type &mask) const
{
    vector<TimeMark>::const_iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), TimeMark(tg.t(),mask));
    while (  ! tg.lt(first_ge->time()) || ! first_ge->match_mask(mask) ) {
        ++first_ge;
    }
    return TimeMarksIterator(marks_, first_ge, mask);
}

TimeMarks::iterator TimeMarks::last(const TimeGovernor &tg, const TimeMark::Type &mask) const
{
    vector<TimeMark>::const_iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), TimeMark(tg.t()+0.01*tg.dt(),mask));
    while ( ! tg.ge(first_ge->time()) || ! first_ge->match_mask(mask) )
        --first_ge;
    return TimeMarksIterator(marks_, first_ge, mask);
}

ostream& operator<<(ostream& stream, const TimeMarks &marks)
{
    stream << "time marks:" << endl;
    for(vector<TimeMark>::const_iterator it = marks.marks_.begin(); it != marks.marks_.end(); ++it)
        stream << *it << endl;
    return stream;
}


vector<TimeMark> TimeMarks::get_marks(){
    return marks_;
};
