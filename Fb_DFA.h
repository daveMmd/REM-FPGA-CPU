//
// Created by 钟金诚 on 2020/7/22.
//

#ifndef BECCHI_REGEX_FB_DFA_H
#define BECCHI_REGEX_FB_DFA_H


#include "dfa.h"
//#include "Fb_NFA.h"

class Fb_DFA {
private:
    int cons2state_num;
    int entry_allocated;
public:
    /* state involving no progression */
    state_t dead_state;

    state_t **state_table;
    int _size;
    int *is_accept;

    Fb_DFA();
    explicit Fb_DFA(DFA* dfa);
    ~Fb_DFA();

    void to_dot(char* fname, char* title);

    /*Brzozowski’s algorithm*/
    Fb_DFA* minimise();

    /*Minimization using Equivalence Theorem*/
    Fb_DFA* minimise2();

    void add_transition(state_t s, int c, state_t d);
    state_t add_state();

    /* returns the next_state from state on symbol c */
    state_t get_next_state(state_t state, int c);

    int size();
    int less2states();
    int cons2states();
    int get_large_states_num();

    Fb_DFA* converge(Fb_DFA *);
    bool small_enough();

    float get_ratio();
};


#endif //BECCHI_REGEX_FB_DFA_H