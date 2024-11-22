#ifndef UNTITLED_MATHEXPRESSION_H
#define UNTITLED_MATHEXPRESSION_H
#include "../Token/Token.h"


class MathExpression {
public:
    std::string value;
    std::string result;
    std::string result_type;
    MathExpression *left_ptr = nullptr, *right_ptr = nullptr;
    virtual void execute() = 0;
};


class AbstractMathExpressionFactory {
public:
    using create_f = std::unique_ptr<AbstractMathExpressionFactory>();

    AbstractMathExpressionFactory() = default;
    ~AbstractMathExpressionFactory() = default;

    static void registrate(std::string const & name, create_f * fp){
        registry[name] = fp;
    }

    virtual MathExpression *get_MathExpression(std::string x);

    static std::unique_ptr<AbstractMathExpressionFactory> make(std::string const & name) {
        auto it = registry.find(name);
        return it == registry.end() ? nullptr : (it->second)();
    }

    template <typename F>
    struct Registrar {
        explicit Registrar(std::string const & name) {
            AbstractMathExpressionFactory::registrate(name, &F::create);
        }
    };
private:
    static std::map<std::string, create_f *> registry;
};


class Int32: public MathExpression {
public:
    explicit Int32(std::string &val){
        this->value = val;
        this->result_type = "int32";
    }

    void execute() override {
        this->result = value;
    }
};

class Int32Factory : public AbstractMathExpressionFactory {
public:
    static std::unique_ptr<AbstractMathExpressionFactory> create() {
        return std::make_unique<AbstractMathExpressionFactory>();
    }

    Int32Factory() = default;
    MathExpression *get_MathExpression(std::string x) override;
    ~Int32Factory () = default;
};

#endif //UNTITLED_MATHEXPRESSION_H
