#ifndef AUTOMATON_H
#define AUTOMATON_H

#include "NormalForm.h"
#include "State.h"
#include "Utilities.h"
#include "WeakSatisfactionChecker.h"
#include "crow_all.h"
#include <queue>
#include <spot/tl/formula.hh>
#include <vector>

class TransitionsSystem
{
  public:
    explicit TransitionsSystem(const spot::formula& formula);
    ~TransitionsSystem();
    bool Build(crow::websocket::connection& conn);

  private:
    spot::formula m_initialFormula;
    std::set<spot::formula> m_statesTrack;

    spot::formula GetFormula(const std::pair<spot::formula, spot::formula>& transition);
    State GetCurrentState(std::queue<spot::formula>& statesQueue, crow::websocket::connection& conn);
    std::set<std::pair<spot::formula, spot::formula>> CalculateTransitions(State& state,
                                                                           crow::websocket::connection& conn);
    spot::formula GetNextState(crow::websocket::connection& conn,
                               const std::pair<spot::formula, spot::formula>& transition);
    void SendSCCInfoMsg(crow::websocket::connection& conn) const;
    spot::formula GetObligationFormula(State& state) const;
    bool HandleSAT(crow::websocket::connection& conn);
    bool CheckWeakSatisfaction(State& state, spot::formula& transitionCondition, crow::websocket::connection& conn);
    bool ContinueExploring(crow::websocket::connection& conn) const;
    bool IsSCC(State& state, const spot::formula& nextState) const;
    std::queue<spot::formula> InitTransitionsSystem();
    void InsertState(std::queue<spot::formula>& statesQueue, const spot::formula& nextState);
};

#endif