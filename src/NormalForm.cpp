#include "NormalForm.h"



NormalForm::NormalForm(const std::shared_ptr<Formula>& formula)
{
	m_formula = formula;
}


NormalForm::~NormalForm()
{
}

void NormalForm::Calculate(){}