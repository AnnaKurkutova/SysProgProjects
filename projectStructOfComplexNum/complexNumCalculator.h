/*Объявление структуры и её функций*/
struct ComplexNum{
	float realPart;
	float imaginaryPart;
};

struct ComplexNum add (struct ComplexNum, struct ComplexNum);
struct ComplexNum subtract (struct ComplexNum, struct ComplexNum);
struct ComplexNum multiply (struct ComplexNum, struct ComplexNum);
struct ComplexNum divide (struct ComplexNum, struct ComplexNum);
void printComplex (struct ComplexNum*);
void scanComplex (struct ComplexNum*);