#include "phone.hpp"

/* Construeix un telèfon a partir del seu número (num), el seu nom
(name) i el seu comptador de trucades (compt). 
Es produeix un error si name no és un identificador legal. */
phone::phone(nat num, const string& name, nat compt) throw(error)
{
    for(nat i = 0; i < name.size(); i++){
        if(name[i] == DELETECHAR or name[i] == ENDCHAR or name[i] == ENDPREF) throw error(ErrNomIncorrecte);
    }
    _numTelf = num;
    _nomP = name;
    _comptTruc = compt;
}

/* Tres grans. Constructor per còpia, operador d'assignació i destructor. */
//CONSTRUCTOR PER CÒPIA//
phone::phone(const phone& T) throw(error) : _numTelf(T._numTelf), _nomP(T._nomP), _comptTruc(T._comptTruc) {}

//OPERADOR ASSIGNACIÓ//
phone& phone::operator=(const phone& T) throw(error) 
{
    _numTelf = T._numTelf;
    _nomP = T._nomP;
    _comptTruc = T._comptTruc;
    return *this;
}

//DESTRUCTOR//
phone::~phone() throw(){}

// Pre:  Cert
// Post: Retorna el número de telèfon.
nat phone::numero() const throw()
{
    return _numTelf;
}

// Pre:  Cert
// Post: Retorna el nom associat al telèfon, eventualment l'string buit.
string phone::nom() const throw()
{
    return _nomP;
}

// Pre:  Cert
// Post: Retorna el número de vegades que s'ha trucat al telèfon.
nat phone::frequencia() const throw()
{
    return _comptTruc;
}

// Operador de preincrement. 
// Pre:  Cert
/* Post: Incrementa en 1 el número de vegades que s'ha trucat al telèfon i
retorna una referència a aquest telèfon. */
phone& phone::operator++() throw()
{
    _comptTruc += 1; 
    return *this; 
}

// Operador de postincrement. 
// Pre:  Cert
/* Post: Incrementa en 1 el número de vegades que s'ha trucat al telèfon i
retorna una còpia d'aquest telèfon sense incrementar. */
phone phone::operator++(int) throw()
{
    phone p2 = *this;
    _comptTruc += 1; 
    return p2; 
    
}

// Operadors de comparació.
// Pre:  Cert
/* Post: L'operador > retorna cert, si i només si, el
telèfon sobre el que s'aplica el mètode és més freqüent que el
telèfon T, o a igual freqüència, el nom associat al telèfon és
major en ordre lexicogràfic que el nom associat a T. 
La resta d'operadors es defineixen consistentment respecte a >. */
bool phone::operator==(const phone& T) const throw()
{
    return ((_comptTruc == T._comptTruc) and (_nomP == T._nomP));
}

bool phone::operator!=(const phone& T) const throw()
{
    return !(*this == T);
}

bool phone::operator<(const phone& T) const throw()
{
    if(_comptTruc == T._comptTruc) return (_nomP < T._nomP);
    return (_comptTruc < T._comptTruc); 
}

bool phone::operator>(const phone& T) const throw()
{
    if(_comptTruc == T._comptTruc) return (_nomP > T._nomP);
    return (_comptTruc > T._comptTruc);
}

bool phone::operator<=(const phone& T) const throw()
{
    if(_comptTruc == T._comptTruc) return (_nomP <= T._nomP);
    return (_comptTruc <= T._comptTruc); 
}

bool phone::operator>=(const phone& T) const throw()
{
    if(_comptTruc == T._comptTruc) return (_nomP >= T._nomP);
    return (_comptTruc >= T._comptTruc);
}
