#define DEBUG_BIT 1
#if DEBUG_BIT
#define LOG(x) std::cout << x << '\n';
#else
#define LOG(x)
#endif

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>

// T: type of states
template <typename T>
class DFA {
    std::set<T> states;
    std::set<char> symbols;
    std::map<std::pair<T, char>, T> transitions;
    T initialState;
    std::set<T> finalStates;

public:
    DFA(
        std::set<T> states,
        std::set<char> symbols,
        std::map<std::pair<T, char>, T> transitions,
        T initialState,
        std::set<T> finalStates
    ) : states{states}, symbols{symbols},
    transitions{transitions}, initialState{initialState}, finalStates{finalStates} {}

    bool accepts(std::string s) {
        T currentState = initialState;
        for (char c : s) {
            if (transitions.contains(std::pair<T, char>{currentState, c})) {
                currentState = transitions[{currentState, c}];
            } else {
                return false;
            }
        }
        return finalStates.contains(currentState);
    }

    void minimizeDFA() {
        // CREATE ALL PAIRS
        std::map<std::pair<T, T>, bool> markedPairs;
        for (auto& state1 : states) {
            for (auto& state2 : states) {
                if (state1 >= state2) continue;
                if ((finalStates.contains(state1) && !finalStates.contains(state2)) || (!finalStates.contains(state1) && finalStates.contains(state2))) {
                    markedPairs[{state1, state2}] = true;
                } else {
                    markedPairs[{state1, state2}] = false;
                }
                // std::cout << "Q_a: " << state1 << " Q_b: " << state2 << " --> " << markedPairs[{state1, state2}] << '\n';
            }
        }

        // Iterate through transitions and mark TRUE
        bool somethingChanged = true;
        while (somethingChanged) {
            somethingChanged = false;

            for (auto const& [key, value] : markedPairs) {
                if (value) continue;

                for (auto& symbol : symbols) {
                    if (transitions.contains({key.first, symbol}) && 
                    transitions.contains({key.second, symbol}) && 
                    markedPairs.contains({transitions[{key.first, symbol}], transitions[{key.second, symbol}]}) && 
                    markedPairs[{transitions[{key.first, symbol}], transitions[{key.second, symbol}]}]) {
                        markedPairs[key] = true;
                        somethingChanged = true;
                    }
                }
            }
        }

        // Combine Unmarked Pairs
        for (auto const& [key, value] : markedPairs) {
            if (!value) {
                T a = key.first;
                T b = key.second;

                states.erase(states.find(b));

                if (b == initialState) initialState = a;
                if (finalStates.contains(b))  {
                    finalStates.erase(finalStates.find(b));
                    finalStates.insert(a);
                }

                std::map<std::pair<T, char>, T> newTransitions;
                for (auto& transition : transitions) {
                    if (transition.second == b) transition.second = a;
                    if (transition.first.first == b) {
                        newTransitions[{a, transition.first.second}] = transition.second;
                    } else {
                        newTransitions[transition.first] = transition.second;
                    }
                }
                transitions = newTransitions;
            }
        } 
    }

    template <typename U>
    friend std::ostream& operator<<(std::ostream& out, const DFA<U>& dfa);
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const DFA<T>& dfa) {
    out << "--- DFA ---" << '\n';
    
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
        out << '{' << transition.first.first << ", " << transition.first.second << "} --> " << transition.second << '\n';
    }

    return out;
}

int main() {
    // DFA where a's must precede b's
    DFA dfa1 = DFA<size_t>( 
        std::set<size_t>{0, 1, 2},
        std::set<char>{'a', 'b'},
        std::map<std::pair<size_t, char>, size_t>{
            {{0, 'a'}, 0},
            {{0, 'b'}, 1},
            {{1, 'a'}, 2},
            {{1, 'b'}, 1},
            {{2, 'a'}, 2},
            {{2, 'b'}, 2},
        },
        0,
        {0, 1}
    );
    // DFA where a's and b's must have same parity
    DFA dfa2 = DFA<size_t>(
        std::set<size_t>{0, 1, 2, 3},
        std::set<char>{'a', 'b'},
        std::map<std::pair<size_t, char>, size_t>{
            {{0, 'a'}, 2},
            {{0, 'b'}, 1},
            {{1, 'a'}, 0},
            {{1, 'b'}, 3},
            {{2, 'a'}, 0},
            {{2, 'b'}, 3},
            {{3, 'a'}, 1},
            {{3, 'b'}, 2}
        },
        0,
        {0, 3}
    );
    // dfa3: 
    DFA dfa3 = DFA<size_t>(
        std::set<size_t>{0, 1, 2, 3, 4, 5},
        std::set<char>{'a', 'b'},
        std::map<std::pair<size_t, char>, size_t>{
            {{0, 'a'}, 1}, {{0, 'b'}, 4},
            {{1, 'a'}, 2}, {{1, 'b'}, 3},
            {{2, 'a'}, 2}, {{2, 'b'}, 2},
            {{3, 'a'}, 2}, {{3, 'b'}, 3},
            {{4, 'a'}, 5}, {{4, 'b'}, 4},
            {{5, 'a'}, 5}, {{5, 'b'}, 4}
        },
        0,
        {2, 3}
    );

    assert(dfa1.accepts(""));
    assert(dfa1.accepts("aabbb"));
    assert(dfa1.accepts("ab"));
    assert(!dfa1.accepts("ba"));
    assert(!dfa1.accepts("aabbab"));
    assert(dfa2.accepts(""));
    assert(dfa2.accepts("ab"));
    assert(dfa2.accepts("ba"));
    assert(!dfa2.accepts("aabba"));

    std::cout << dfa3 << "\n";

    // dfa1.minimizeDFA();
    // dfa2.minimizeDFA();
    dfa3.minimizeDFA();

    std::cout << dfa3 << "\n";
}
