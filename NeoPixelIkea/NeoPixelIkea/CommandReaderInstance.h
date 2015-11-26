#pragma once

CommandReader cr(Serial);
// Short names for printing
template<typename A> 
inline void _(A const &x) { cr._(x); };
template<typename A>
inline void _n(A const &x) { cr._n(x); }
