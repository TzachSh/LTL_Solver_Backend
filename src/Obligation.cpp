#include "Obligation.h"

Obligation::Obligation(spot::formula formula) : m_formula { std::move(formula) } {}

Obligation::~Obligation() = default;