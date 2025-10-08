#include <List.h>
namespace pr {

// ******************* Chainon
//faute list.h ppas inclu
Chainon::Chainon (const std::string & data, Chainon * next):data(data),next(next) {};
//faute list.h ppas inclu
size_t Chainon::length() {
	size_t len = 1;
	if (next != nullptr) {
		len += next->length();
	}
	//faute len
	return len;
}
//faute il manque const pour la signature
void Chainon::print (std::ostream & os) const {
	os << data ;
	if (next != nullptr) {
		os << ", ";
		//faute si il est null
		next->print(os);
	}
	
}

// ******************  List
const std::string & List::operator[] (size_t index) const  {
	Chainon * it = tete;
	for (size_t i=0; i < index ; i++) {
		it = it->next;
	}
	return it->data;
}

void List::push_back (const std::string& val) {
	if (tete == nullptr) {
		tete = new Chainon(val);
	} else {
		Chainon * fin = tete;
		while (fin->next) {
			fin = fin->next;
		}
		fin->next = new Chainon(val);
	}
}
//faute definition deux une dans .h une ici
void List::push_front (const std::string& val) {
	tete = new Chainon(val,tete);
}
//faute linkage
bool List::empty() {
	return tete == nullptr;
}

size_t List::size() const {
	if (tete == nullptr) {
		return 0;
	} else {
		return tete->length();
	}
}
//faute il était en dehors du namespace
std::ostream & operator<< (std::ostream & os, const pr::List & vec)
{
	os << "[";
	if (vec.tete != nullptr) {
		vec.tete->print (os) ;
	}
	os << "]";
	return os;
}

} // namespace pr

