#define DEBUG_BIT 1
#if DEBUG_BIT
#define LOG(x) std::cout << x << '\n';
#else
#define LOG(x)
#endif

#include <iostream>
#include <map>
#include <set>
#include <vector>

// we use ASCII 0 (NULL) as out EPSILON value
template <typename T>
class epsilonNFA {
    std::set<T> states;
    std::set<char> symbols;
    std::map<std::pair<T, char>, std::set<T>> transitions;
    T initialState;
    std::set<T> finalStates;
    std::map<T, std::set<T>> stateEpsilonClosures;

public:
    epsilonNFA(
        std::set<T> states,
        std::set<char> symbols,
        std::map<std::pair<T, char>, std::set<T>> transitions,
        T initialState,
        std::set<T> finalStates
    ) : states{states}, symbols{symbols},
    transitions{transitions}, initialState{initialState}, finalStates{finalStates} {
        symbols.insert(0);
        for (T state : states) {
            stateEpsilonClosures[state] = calculateEpsilonClosure(state);
        }
    }

    bool accepts(std::string s) {
        std::set<T> currentStates;
        std::set<T> nextStates;
        currentStates.insert(initialState);
        for (T epsilonReachableState : stateEpsilonClosures[initialState]) {
            currentStates.insert(epsilonReachableState);
        }
        
        for (char c : s) {
            for (T currentState : currentStates) {
                if (transitions.contains({currentState, c})) {
                    for (T nextState : transitions[{currentState, c}]) {
                        nextStates.insert(nextState);
                        for (T epsilonReachableState : stateEpsilonClosures[nextState]) {
                            nextStates.insert(epsilonReachableState);
                        }
                    }
                }
            }
            currentStates = nextStates;
            nextStates.clear();
        }

        for (T state : currentStates) {
            if (finalStates.contains(state))
                return true;
        }
        
        return false;
    }

    std::set<T> calculateEpsilonClosure(T fromState) {
        std::vector<T> stateQ;
        std::set<T> epsilonSet;
        stateQ.emplace_back(fromState);
        int i = 0, j = 1;

        while (i < j) {
            fromState = stateQ[i];
            if (!transitions.contains({fromState, 0})) {
                ++i;
                continue;
            }
            epsilonSet = transitions[{fromState, 0}];

            for (T toState : epsilonSet) {
                if (std::find(stateQ.begin(), stateQ.end(), toState) == stateQ.end()) {
                    stateQ.emplace_back(toState);
                    ++j;
                }
            }
            ++i;
        }

        return std::set<T>(stateQ.begin(), stateQ.end());
    }

    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const epsilonNFA<U>& enfa);
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const epsilonNFA<T>& enfa) {
    out << "--- eNFA ---" << '\n';
    
    out << "STATES:";
    for (auto& state : enfa.states) {
        out << " " << state;
    }
    out << '\n' << "INITIAL STATE: " << enfa.initialState << '\n';
    out << "FINAL STATES:";
    for (auto& state : enfa.finalStates) {
        out << " " << state;
    }
    out << '\n';

    out << "TRANSITIONS:" << '\n';
    for (auto& transition : enfa.transitions) {
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

    transitions1[{0, '0'}] = {0};
    transitions1[{0, 0}] = {1};
    transitions1[{1, '0'}] = {2};
    transitions1[{2, '1'}] = {1};
    transitions1[{1, 0}] = {3};
    transitions1[{3, '0'}] = {3};
    
    // NFA where a's must precede b's in a non-empty string
    epsilonNFA enfa1 = epsilonNFA<size_t>( 
        std::set<size_t>{0, 1, 2, 3},
        std::set<char>{'0', '1'},
        transitions1,
        0,
        {3}
    );

    assert(enfa1.accepts(""));
    assert(enfa1.accepts("000"));
    assert(enfa1.accepts("01"));
    assert(!enfa1.accepts("011"));
    assert(!enfa1.accepts("1"));
    

    std::cout << enfa1 << '\n';
}
