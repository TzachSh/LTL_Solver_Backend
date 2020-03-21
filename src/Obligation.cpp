#include "Obligation.h"

#include <utility>

Obligation::Obligation(spot::formula formula) : m_formula { std::move(formula) } {}

Obligation::~Obligation() = default;