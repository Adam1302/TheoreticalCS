#define DEBUG_BIT 1
#if DEBUG_BIT
#define LOG(x) std::cout << x << '\n';
#else
#define LOG(x)
#endif

#include <iostream>
#include <map>
#include <set>

// we use ASCII 0 (NULL) as out EPSILON value
template <typename T>
class NFA {
    std::set<T> states;
    std::set<char> symbols;
    std::map<std::pair<T, char>, std::set<T>> transitions;
    T initialState;
    std::set<T> finalStates;

public:
    NFA(
        std::set<T> states,
        std::set<char> symbols,
        std::map<std::pair<T, char>, std::set<T>> transitions,
        T initialState,
        std::set<T> finalStates
    ) : states{states}, symbols{symbols},
    transitions{transitions}, initialState{initialState}, finalStates{finalStates} {}

    bool accepts(std::string s) {
        std::set<T> currentStates;
        std::set<T> nextStates;
        currentStates.insert(initialState);
        
        for (char c : s) {
            for (T currentState : currentStates) {
                if (transitions.contains({currentState, c})) {
                    for (T nextState : transitions[{currentState, c}])
                        nextStates.insert(nextState);
                } else {
                    return false;
                }
            }
            currentStates = nextStates;
            nextStates.clear();
        }
        for (T state : currentStates) {
            if (finalStates.contains(state)) return true;
        }
        return false;
    }

    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const NFA<U>& dfa);
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const NFA<T>& dfa) {
    out << "--- NFA ---" << '\n';
    
    out << "STATES:";
    for (auto& state : dfa.states) {
        out << " " << state;
    }
    out << '\n' << "INITIAL STATE: " << dfa.initialState << '\n';
    out << "FINAL STATES:";
    for (auto& state : dfa.finalStates) {
        out << " " << state;
    }
    out << '\n';

    out << "TRANSITIONS:" << '\n';
    for (auto& transition : dfa.transitions) {
        out << '{' << transition.first.first << ", " << transition.first.second << "} --> " << "{";
        for (T state : transition.second) {
            out << " " << state;
        }
        out << " }" << '\n';
    }

    return out;
}

int main() {
    std::map<std::pair<size_t, char>, std::set<size_t>> transitions1;

    transitions1[{0, 'a'}] = {0, 3};
    transitions1[{0, 'b'}] = {1, 4};
    transitions1[{1, 'a'}] = {2};
    transitions1[{1, 'b'}] = {1, 4};
    transitions1[{2, 'a'}] = {2};
    transitions1[{2, 'b'}] = {2};
    transitions1[{3, 'a'}] = {0, 3};
    transitions1[{3, 'b'}] = {1, 4};
    transitions1[{4, 'a'}] = {2};
    transitions1[{4, 'b'}] = {4};
    
    // NFA where a's must precede b's in a non-empty string
    NFA nfa1 = NFA<size_t>( 
        std::set<size_t>{0, 1, 2, 3, 4},
        std::set<char>{'a', 'b'},
        transitions1,
        0,
        {3, 4}
    );

    assert(!nfa1.accepts(""));
    assert(nfa1.accepts("aabbb"));
    assert(nfa1.accepts("ab"));
    assert(!nfa1.accepts("ba"));
    assert(!nfa1.accepts("aabbab"));

    std::cout << nfa1 << '\n';
}
