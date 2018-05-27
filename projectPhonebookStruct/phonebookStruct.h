//объявление структуры и функций для работы с данной структурой
struct Phonebook{
	char name[10];
	char surname[10];
	char telephoneNumber[12];
};

struct Phonebook *addNewElem(struct Phonebook *, int*);
void printPhonebook(struct Phonebook *, int);
int searchByTelephoneNumber(struct Phonebook *,int);
struct Phonebook *deleteElem(struct Phonebook *, int*);