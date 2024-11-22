#include "MathExpression.h"

namespace
{
    Int32Factory::Registrar<Int32Factory> reg1("int32");
}

MathExpression *Int32Factory::get_MathExpression(std::string x) {
    MathExpression *ret = new Int32(x);
    return ret;
}