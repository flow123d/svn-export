/*
 * time_marks.cc
 *
 *  Created on: Jun 15, 2011
 *      Author: jb
 */

#include <system/system.hh>
#include <time_marks.hh>
#include <time_governor.hh>
#include <algorithm>

ostream& operator<<(ostream& stream, const TimeMark &mark)
{
    return ( stream << mark.time()<<": " << hex << mark.mark_type() );
}

const TimeMark::Type TimeMark::strict =  0x1;
const TimeMark::Type TimeMark::every_type =  ~0x0;

TimeMark::Type TimeMarks::new_mark_type() {
    ASSERT(next_mark_type_ != 0, "Can not allocate new mark type. The limit is 32 mark types.\n");
    TimeMark::Type current_type = next_mark_type_;

    next_mark_type_ <<= 1;
    return current_type;
}

TimeMark::Type TimeMarks::new_strict_mark_type() {
    return new_mark_type() | TimeMark::strict;
}

void TimeMarks::add(const TimeMark &mark) {
    vector<TimeMark>::iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), mark);
    marks_.insert(first_ge, mark);
}

void TimeMarks::add_time_marks(double time, double dt, double end_time, TimeMark::Type type) {
    for (double t = time; t <= end_time; t += dt)
        add(TimeMark(t, type));
}

bool TimeMarks::is_current(const TimeGovernor &tg, const TimeMark::Type &mask) const
{
    TimeMark tm = *last(tg, mask);
    return tg.le(tm.time() + tg.dt());
}

TimeMarks::iterator TimeMarks::next(const TimeGovernor &tg, const TimeMark::Type &mask) const
{

    vector<TimeMark>::const_iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), TimeMark(tg.t(),mask));
    while (  ! tg.lt(first_ge->time()) || ! first_ge->match_mask(mask) ) {
        //cout << *first_ge;
        //DBGMSG(" iter: %d %d %d\n",first_ge - marks_.begin(), tg.lt(first_ge->time()), tg.t() < first_ge->time());
        ++first_ge;
    }
    return TimeMarksIterator(marks_, first_ge, mask);
}

TimeMarks::iterator TimeMarks::last(const TimeGovernor &tg, const TimeMark::Type &mask) const
{
    vector<TimeMark>::const_iterator first_ge = std::lower_bound(marks_.begin(), marks_.end(), TimeMark(tg.t(),mask));
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