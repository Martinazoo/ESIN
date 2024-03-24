#include "call_registry.hpp"

// Pre:  Cert
// Post: S'ha construït un call_registry buit.
// Cost: Θ(n), és a dir, cost lineal.

call_registry::call_registry() throw(error): _quants(0)
{
    _M = 11;
    _taula = new node_hash *[_M];
    for(int i = 0; i < _M ; i++){
        _taula[i] = nullptr;
    }

}

// Tres grans. Constructor per còpia, operador d'assignació i destructor.
//CONSTRUCTOR PER CÒPIA
// Pre:  Cert
// Post: S'ha construït un call_registry igual al call_registry R.
// Cost: Θ(n), és a dir, cost lineal.
call_registry::call_registry(const call_registry& R) throw(error)
{
    _M = R._M;
    _quants = R._quants;
    _taula = new node_hash *[R._M];

    for(nat i = 0; i < R._M; i++){
        _taula[i] = nullptr;
        node_hash *p = R._taula[i];
        node_hash *pant = nullptr;
        while(p != nullptr){
            node_hash *pnou = new node_hash;
            pnou->_ph = p->_ph;
            pnou->_seg = nullptr;

            if(pant == nullptr) _taula[i] = pnou;
            else pant->_seg = pnou;

            pant = pnou;
            p = p->_seg;
        }
    }
}

//OPERADDOR D'ASSIGNACIÓ
// Pre:  Cert
// Post: S'ha construit un call_registry igual al call_registry R.
// Cost: Θ(n), és a dir, cost lineal.
call_registry& call_registry::operator=(const call_registry& R) throw(error)
{
    this->~call_registry();
    _M = R._M;
    _quants = R._quants;
    _taula = new node_hash *[R._M];

    for(nat i = 0; i < R._M; i++){
        _taula[i] = nullptr;
        node_hash *p = R._taula[i];
        node_hash *pant = nullptr;
        while(p != nullptr){
            node_hash *pnou = new node_hash;
            pnou->_ph = p->_ph;
            pnou->_seg = nullptr;

            if(pant == nullptr) _taula[i] = pnou;
            else pant->_seg = pnou;

            pant = pnou;
            p = p->_seg;
        }
    }

    return *this;
}

//DESTRUCTOR
// Pre: ∃ existeix un call_registry
// Post: s'ha destruit tot el call_registry implícit.
// Cost: Θ(n), és a dir, cost lineal.
call_registry::~call_registry() throw() 
{
    for (nat i = 0; i < _M; i++) {
        esborra_nodes(_taula[i]);
    }
    delete[] _taula;
}


// Pre:  Cert
/* Post: S'ha registrat que s'ha realitzat una trucada al número donat, 
incrementant en 1 el comptador de trucades associat. Si el número no 
estava prèviament en el call_registry afegeix una nova entrada amb 
el número de telèfon donat, l'string buit com a nom i el comptador a 1. */
// Cost: Θ(n), és a dir, cost lineal.
void call_registry::registra_trucada(nat num) throw(error)
{
    long i = hash_func(num) %_M;
    node_hash *p = _taula[i];
    bool trobat = false;

    while(p != nullptr and not trobat){
        if(p->_ph.numero() == num){
            trobat = true;
            p->_ph++;
        }
        else p = p->_seg;
    }
    
    if(not trobat){
        phone phnou(num, "", 1);
        insereix(phnou);

        if(factor_carrega() > 0.8) {
            nat M2 = _M*2+1;
            redispersio(M2);
        }
    }
}

// Pre:  Cert
/* Post: S'ha assignat el nom indicat al número donat.
Si el número no estava prèviament en el call_registry, s'afegeix
una nova entrada amb el número i nom donats, i el comptador 
de trucades a 0. 
Si el número existia prèviament, se li assigna el nom donat. */
void call_registry::assigna_nom(nat num, const string& name) throw(error)
{
    long i = hash_func(num) %_M;
    node_hash *p = _taula[i];
    bool trobat = false;

    while(p != nullptr and not trobat){
        if(p->_ph.numero() == num){
            trobat = true;
            phone pnou(num, name, p->_ph.frequencia());
            p->_ph = pnou;
        }
        else p = p->_seg;
    }

    if(not trobat){
        phone phnou(num, name, 0);
        insereix(phnou);
        
        if(factor_carrega() > 0.8) {
            nat M2 = _M*2+1;
            redispersio(M2);
        }
    }    
}

// Pre:  Cert
/* Post: S'ha eliminat l'entrada corresponent al telèfon el número num.
Es produeix un error si el número no estava present. */
// Cost: Θ(n), és a dir, cost lineal.
void call_registry::elimina(nat num) throw(error)
{
    long i = hash_func(num) % _M;
    node_hash *p = _taula[i];
    node_hash *pant = nullptr;
    bool trobat = false;
    while(p != nullptr and not trobat){
        if(p->_ph.numero() == num){
            trobat = true;
            if(pant == nullptr){
                _taula[i] = p->_seg;
            }
            else {
                pant->_seg = p->_seg;
            }
            delete(p);
            _quants--;
        }
        else{
            pant = p;
            p = p->_seg;
        }    
    }
    if(not trobat) throw error(ErrNumeroInexistent);
    else{
        if(factor_carrega() < 0.2) {
            nat M2 = _M/2;
            redispersio(M2);
        }
    }
}

// Pre:  Cert
/* Post: Retorna cert si i només si el call_registry conté un 
telèfon amb el número donat. */
// Cost: Θ(1), és a dir, cost constant.
bool call_registry::conte(nat num) const throw()
{
    long i = hash_func(num) % _M;
    node_hash *p = _taula[i];
    bool trobat = false;
    while(p != nullptr and not trobat){
        if(p->_ph.numero() == num) trobat = true;
        else 
            p = p->_seg;
    }
    return trobat;
}

// Pre:  Cert
/* Post: Retorna el nom associat al número de telèfon que s'indica.
Aquest nom pot ser l'string buit si el número de telèfon no
té un nom associat. Es produeix un error si el número no està en
el call_registry. */
// Cost: Θ(1), és a dir, cost constant.
string call_registry::nom(nat num) const throw(error)
{
    long i = hash_func(num) % _M;
    string res;
    node_hash *p = _taula[i];
    bool trobat = false;
    while(p != nullptr and not trobat){
        if(p->_ph.numero() == num){
            trobat = true;
            res = p->_ph.nom();
        }
        else{
            p = p->_seg;
        }
    }
    if(not trobat) throw error(ErrNumeroInexistent);
    return res;
}

// Pre:  Cert
/* Post: Retorna el comptador de trucades associat al número de telèfon 
indicat. Aquest número pot ser 0 si no s'ha efectuat cap trucada a
aquest número. Es produeix un error si el número no està en el 
call_registry. */
// Cost: Θ(1), és a dir, cost constant.
nat call_registry::num_trucades(nat num) const throw(error)
{
    nat res;
    long i = hash_func(num) % _M;
    node_hash *p = _taula[i];
    bool trobat = false;
    while (p != nullptr and not trobat){
        if(p->_ph.numero() == num){
            trobat = true;
            res = p->_ph.frequencia();
        }
        else{
            p = p->_seg;
        }
    }
    if(not trobat)throw error(ErrNumeroInexistent);
    return res;
}

// Pre:  Cert
// Post: Retorna cert si i només si el call_registry està buit.
// Cost: Θ(1), és a dir, cost constant.
bool call_registry::es_buit() const throw()
{
    return (_quants == 0);
}

// Pre:  Cert
// Post: Retorna quants números de telèfon hi ha en el call_registry.
// Cost: Θ(1), és a dir, cost constant.
nat call_registry::num_entrades() const throw()
{
    return _quants;
}


// Pre:  Cert
/* Post: El vector de phone conté un bolcat de totes les entrades que
tenen associat un nom no buit.
Es produeix un error en cas de que no tots els noms dels telèfons
siguin diferents. */
// Cost: Θ(n²), és a dir, cost quadràtic.
void call_registry::dump(vector<phone>& V) const throw(error)
{
    for(nat i = 0; i < _M; i++){
        node_hash *p = _taula[i];
        while(p != nullptr){
            if(p->_ph.nom() != ""){
                for(nat i = 0; i < V.size(); i++){ 
                    if(V[i].numero() == p->_ph.numero()) throw error(ErrNumeroInexistent);
                    if(V[i].nom() == p->_ph.nom()) throw error (ErrNomRepetit);
                }
                V.push_back(p->_ph);
            } 
            p = p->_seg;
        }
    }
}

// Pre:  Cert
// Post: Retorna l'index y de la hash table al que s'ha d'accedir depenent del numero x
// Cost: Θ(1), és a dir, cost constant.
long call_registry::hash_func(nat x)
{
    long y = (( x * x * MULT ) << 20) >> 4;
    if(y < 0)
        y = -y;
    return y ;
}

// Pre: Cert
// Post: S'ha eliminat tota la llista de phones a la que apunta p
// Cost: Θ(1), és a dir, cost constant.
void call_registry::esborra_nodes(node_hash *p){
    if(p != nullptr){
        esborra_nodes(p->_seg);
        delete p;
    }
}

// Pre: Cert
// Post: s'ha modificat la mida de la taula de c2, la seva mida serà M.
// Cost: Θ(n), és a dir, cost lineal.
void call_registry::modificar_mida_taula(call_registry &c2, nat M)
{
    for(nat i = 0; i < c2._M; i++){
        esborra_nodes(c2._taula[i]);
    }
    delete[] c2._taula;

    c2._M = M;

    c2._taula = new node_hash *[M];
    for(nat i = 0; i < M ; i++){
        c2._taula[i] = nullptr;
    }

    
}

// Pre:  Cert
// Post: s'ha inserit el phone ph que no es troba a la hash table incrementant en 1 el
// numero d'elements que hi ha a la taula.
// Cost: Θ(n), és a dir, cost lineal.
void call_registry::insereix(const phone &ph)
{
    long i = hash_func(ph.numero()) % _M;
    _taula[i] = new node_hash(ph, _taula[i]);
    _quants++;
}

// Pre:  Cert
// Post:  Retorna el factor de càrrega de la taula de dispersió, el factor càrrega depén de com
// de plena es la nostra taula i de la seva grandària.
// Cost: Θ(1), és a dir, cost constant.
float call_registry::factor_carrega()
{
    return (float)_quants / (float) _M;   
}

// Pre: Factor de càrrega > 0.8 o Factor de càrrega < 0.2
// Post: s'ha redimensionat la mida de la taula de disersió a M2
// depenent del factor de càrrega, amb tots els seus elements. 
// Cost: Θ(n), és a dir, cost lineal.
void call_registry::redispersio(nat &M2)
{
    call_registry c2;
    modificar_mida_taula(c2, M2);

    for(int j = 0; j < _M; ++j){
        if(_taula[j] != nullptr){
            node_hash *paux = _taula[j];
            while(paux != nullptr){
                long i = hash_func(paux->_ph.numero()) % c2._M;
                node_hash *pnou = new node_hash;
                pnou->_ph = paux->_ph;
                pnou->_seg = nullptr;

                if (c2._taula[i] == nullptr) {
                    c2._taula[i] = pnou;
                } else {
                    pnou->_seg = c2._taula[i];
                    c2._taula[i] = pnou;
                }

                paux = paux->_seg;
            }
        }
    }

    swap(_taula, c2._taula);
    swap(_M, c2._M);
}

//CONSTRUCTORS NODE_HASH

// Pre:  Cert
// Post:  S 'ha creat un node_hash buit
// Cost: Θ(1), és a dir, cost constant.
call_registry::node_hash::node_hash(): _seg(nullptr)
{

}

//CONSTRUCTOR PER CÒPIA
// Pre:  Cert
// Post: S'ha creat un node_hash nou a partir d'un altre node nh i un phone p
// Cost: Θ(1), és a dir, cost constant.
call_registry::node_hash::node_hash(const phone &p, node_hash *seg): _ph(p), _seg(seg)
{

}
