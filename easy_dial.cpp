#include "easy_dial.hpp"

// Pre:  El call_registry no conté telèfons repetits
/* Post: S'ha construït un easy_dial a partir de la 
informació continguda en el call_registry donat. El
prefix en curs queda indefinit. */
// Cost: Θ(n²), és a dir, cost quadràtic.
easy_dial::easy_dial(const call_registry& R) throw(error) :_arrel(nullptr),  _indefinit(true), _actual(nullptr), _longitudM(0)
{
  vector<phone> res;
  R.dump(res);
  ordena_per_fusio(res);  
  if(res.size() > 0) _pharray = new visita[res.size()];
  _size = res.size();

  float freqtot = 0.0; 
  for(nat i = 0; i < res.size(); ++i){ 
    freqtot += res[i].frequencia();
    visita v(res[i]);
    _pharray[i] = v;
    insereix(_pharray[i]);
  }
  
  vector<nat> pul = pulsacions(res);
  for(nat i = 0; i < res.size(); i++){
    float freq = (float) res[i].frequencia();
    _longitudM += (float) ((freq / freqtot) * pul[i]);
  }  
}

/* Tres grans. Constructor per còpia, operador d'assignació i destructor. */
//CONSTRUCTOR PER CÒPIA
// Pre:  Cert
// Post: S'ha construït un easy_dial nou igual al easy_dial D 
// Cost: Θ(n), és a dir, cost lineal respecte el número de noms emmagatzemats.
easy_dial::easy_dial(const easy_dial& D) throw(error)
{
  _arrel = copianode(D._arrel);
  _prefix = D._prefix;
  _indefinit = D._indefinit;
  _ultimResultat = D._ultimResultat;
  _actual = D._actual;
  _longitudM = D._longitudM;
  _pharray = D._pharray;
  _listNodes = D._listNodes;

}

//OPERADOR D'ASSIGNACIÓ
// Pre:  Cert
// Post: S'ha construït un easy_dial nou igual al easy_dial D 
// Cost: Θ(n), és a dir, cost lineal respecte el número de noms emmagatzemats.
easy_dial& easy_dial::operator=(const easy_dial& D) throw(error)
{
  if(this != &D){
    esborranodes(_arrel);

    _arrel = copianode(D._arrel);
    _prefix = D._prefix;
    _indefinit = D._indefinit;
    _ultimResultat = D._ultimResultat;
    _actual = D._actual;
    _longitudM = D._longitudM;

    if(D._pharray != nullptr){
      _pharray = new visita[_size];
      for(nat i = 0; i < _size; i++){
        _pharray[i]  = D._pharray[i];
      }
    }
    else _pharray = nullptr;
  }
  return *this; 
}

//DESTRUCTOR
// Pre:  Cert
// Post: S'ha esborrat tot l'easy_dial
// Cost: Θ(n), és a dir, cost lineal respecte el número de noms emmagatzemats.
easy_dial::~easy_dial() throw()
{ 
  esborranodes(_arrel);
  //delete[] _pharray;
  _actual = nullptr;
  _prefix = "";
  _indefinit = false;
  _ultimResultat = "";
  _listNodes.reinicia();
  _longitudM = 0.0;
  _size = 0;
  _pharray = nullptr;
}

// Pre:  Cert
/* Post: S'ha inicialitzat el prefix en curs a buit. Retorna el nom de F(S, '');
si F (S, '') no existeix llavors retorna l'string buit. */
// Cost: Θ(1), és a dir, cost constant.
string easy_dial::inici() throw()
{
  _prefix = "";
  _indefinit = false;
  string res;
  if(_arrel == nullptr) return res = "";
  else {
    _actual = _arrel;
    res = _pharray[0].p.nom();    
    _ultimResultat = res;
    _listNodes.reinicia();
    _pharray[0].visitat = true;
    _listNodes.insertInList(_actual, &_pharray[0]);
    }
  return res;
}

// Pre:  Cert
/* Post: Retorna el nom de F(S, p') on p' és el prefix resultant d'afegir
el caràcter c al final del prefix en curs p i fa que el nou prefix en curs
sigui p'.
Si F(S, p) existeix però F(S, p') no existeix llavors retorna l'string buit. 
Si no existeix F(S, p) (i per tant tampoc pot existir F(S, p'))
llavors es produeix un error i el prefix en curs queda indefinit. 
Naturalment, es produeix un error si el prefix en curs inicial p 
fos indefinit. */
// Cost: Θ(1), és a dir, cost constant respecte el nombre de noms al TST.
string easy_dial::seguent(char c) throw(error)
{
  if(_indefinit) throw error(ErrPrefixIndef);  
  if(_ultimResultat == ""){
    _indefinit = true;
    throw error(ErrPrefixIndef);
  }

  if(_arrel == nullptr) return "";
  else {
    _prefix = _prefix + c;
    nat max = 0;
    node_tst *n = nullptr;
    
    if(c == phone::ENDPREF){
      node_tst* aux = _actual->_esq;
      if(aux != nullptr){
        if(aux->_c == phone::ENDPREF and not aux->_pv->visitat){
          _listNodes.insertInList(aux, aux->_pv);
          _ultimResultat = aux->_pv->p.nom();
          aux->_pv->visitat = true;
          _actual = aux;
        }
        else _ultimResultat = "";
      }
      else _ultimResultat = "";
    } 

    else {
      n = _actual;
      busca_max_freq(n, max, c);
      if(n == nullptr) _ultimResultat = "";
      else {
        _listNodes.insertInList(_actual, n->_pv);
        //n->_pv->visitat = true;
        _ultimResultat = n->_pv->p.nom();
      }
    }
    return _ultimResultat;
  } 
}

// Pre:  Cert
/* Post: S'ha eliminat l'últim caràcter del prefix en curs p = p' · a
(a és el caràcter eliminat). Retorna el nom F(S, p') 
i fa que el nou prefix en curs sigui p'. 
Es produeix un error si p fos buida i es fa que el prefix en curs
quedi indefinit. Òbviament, també es produeix un error 
si p fos indefinit. */
// Cost: Θ(1), és a dir, cost constant.
string easy_dial::anterior() throw(error)
{
  if(_indefinit) throw error(ErrPrefixIndef);

  if(_prefix == "") {
    _indefinit = true;
    throw error(ErrNoHiHaAnterior);
  }

  _prefix.pop_back();
  if(_ultimResultat != ""){
    _listNodes.getFin()->_v->visitat = false;
    _listNodes.deleteInList();
  }
  
  if(_listNodes.getSize() > 0){
    _actual = _listNodes.getFin()->_act;
    _ultimResultat = _listNodes.getFin()->_v->p.nom();
    
  }
  else {
    _ultimResultat = "";
  }

  return _ultimResultat;
}

// Pre:  Cert
/* Post: Retorna el número de telèfon de F(S, p), sent p
el prefix en curs. Es produeix un error si p és indefinit o si
no existeix F(S, p). */
// Cost: Θ(1), és a dir, cost constant.
nat easy_dial::num_telf() const throw(error)
{
  if(_indefinit) throw error(ErrPrefixIndef);
  if(_ultimResultat == "") throw error(ErrNoExisteixTelefon);
  return _listNodes.getFin()->_v->p.numero();
}

// Pre:  Cert
/* Post: Retorna en el vector result tots els noms dels contactes de 
telèfon que comencen amb el prefix pref, en ordre lexicogràfic creixent. */
// Cost: Θ(k.length() · log(#simbols))
void easy_dial::comencen(const string& pref, vector<string>& result) const throw(error)
{
  node_tst *n = _arrel;
  nat i = 0;
  while(n != nullptr and i < pref.size()){
    if(n->_c > pref[i]) n = n->_esq;
    else if(n->_c < pref[i]) n = n->_dre;
    else {
      n = n->_cen;
      i++;
    }
  }

  comencen_aux(n, pref, result);
}

// Pre:  Cert
/* Post: Retorna el número mitjà de pulsacions necessàries para obtenir un
telèfon. Formalment, si X és el conjunt de noms emmagatzemats en
el easy_dial i t(s) és el número de pulsacions mínimes
necessàries (= número de crides a l'operació seguent) per
obtenir el telèfon el nom del qual és s. La funció retorna la suma
    Pr(s) · t(s)
per tots els telèfons s del conjunt X, sent Pr(s) la probabilitat de
telefonar a s. La probabilitat Pr(s) s'obté dividint la freqüència de s per
la suma de totes les freqüències. */
// Cost: Θ(1), és a dir, cost constant.
double easy_dial::longitud_mitjana() const throw()
{
  return _longitudM;
}

// Pre:  Cert
// Post: Retorna un punter que té la mateixa estructura (TST) que l'estructura (TST) que 
// es apuntada per t
// Cost: Θ(n), és a dir, cost lineal respecte el número de noms emmagatzemats.
typename easy_dial::node_tst* easy_dial::copianode(node_tst* t)
{
  node_tst *paux = nullptr;
  if(t != nullptr){
    paux = new node_tst(t->_c);
    if(t->_c == phone::ENDPREF){
      paux->_pv = t->_pv;
    }
    paux->_esq = copianode(t->_esq);
    paux->_cen = copianode(t->_cen);
    paux->_dre = copianode(t->_dre);
  }
  return paux;
}

// Pre:  Cert
// Post: s'han esborrat tots els nodes que són fills de t
// Cost: Θ(k.length() · log(#simbols))
void easy_dial::esborranodes(node_tst *t)
{
  if(t != nullptr){
    esborranodes(t->_esq);
    esborranodes(t->_cen);
    esborranodes(t->_dre);
    delete t; 
  }
}

// Pre:  Cert
// Post: S'ha inserit alTST del parametre implicit el nom de la visita v
// Cost: Θ(k.length() · log(#simbols))
void easy_dial::insereix(visita &v)
{
  _arrel = insereix(_arrel, 0, v, v.p.nom() + phone::ENDPREF);
}

// Pre:  Cert
/* Post: Insereix una paraula en el diccionari. Cal tenir
  present que cal afegir un sentinella al final de la paraula que és \0. 
  Retorna phone::ENDPREF.
*/
// Cost: Θ(k.length() · log(#simbols))
typename easy_dial::node_tst* easy_dial::insereix(node_tst *n, nat i, visita &v, const string &k)
{
  if(n == nullptr){
    n = new node_tst(k[i]);
    try{
      if(i < k.length()-1) n->_cen = insereix(n->_cen, i+1, v, k);
      else if(i == k.length() - 1) n->_pv = &v;
    }
    catch(...){
      delete n;
      throw;
    }
  }
  else {
    
    if(n->_c > k[i]) n->_esq = insereix(n->_esq, i, v, k);
    else if(n->_c < k[i]) n->_dre = insereix(n->_dre, i, v, k);
    else{
      n->_cen = insereix(n->_cen, i+1, v, k);
    }
  }
  return n;
}

// Pre:  Cert
// Post: insereix a result totes les paraules que hi ha en el subarbre n.
// Cost: Θ(x.length() · log(#simbols))
void easy_dial::comencen_aux(node_tst *n, const string &pref, vector<string> &result) throw(error)
{
  if(n != nullptr){
    comencen_aux(n->_esq, pref, result);
    
    if(n->_c == phone::ENDPREF) result.push_back(pref);
    else comencen_aux(n->_cen, pref + n->_c, result);
    comencen_aux(n->_dre, pref, result);
  } 
}

// Pre:  Cert
// Post: n apunta al node màxima frequència o a nullptr
// Cost: Θ(h · x.length() · log(#simbols)), sent x el size de la paraula que volem trobar i h l'altura de l'arbre.
void easy_dial::busca_max_freq(node_tst* &n, nat &max, const char &c)
{
  nat  i = 0;
  while(n != nullptr and i < 1){
      if(n->_c > c) n = n->_esq;
      else if(n->_c < c) n = n->_dre;
      else {
        n = n->_cen;
        i++;
      }
    }
  _actual = n;

  if(n == nullptr) return;
  else {
    node_tst* paux = nullptr;
    busca_max_freq_aux(n, max, paux);
    if(paux != nullptr) {
        paux->_pv->visitat = true;
      }
    n = paux;
  } 

}

// Pre:  Cert
// Post: paux apunta al phone de més frequència.
// Cost: Θ(h · x.length() · log(#simbols)), sent x el size de la paraula que volem trobar i h l'altura de l'arbre.
void easy_dial::busca_max_freq_aux(node_tst* n, nat &max, node_tst* &paux)
{
  if(n == nullptr) return;
  else {
    if(n->_c == phone::ENDPREF){
      if(n->_pv->p.frequencia() >= max and !n->_pv->visitat){
        max = n->_pv->p.frequencia();
        paux = n;
      }
    }
    busca_max_freq_aux(n->_esq, max, paux);
    busca_max_freq_aux(n->_dre, max, paux);
    busca_max_freq_aux(n->_cen, max, paux);
    
  }
}

// Pre:  Cert
// Post: retorna un vector on en cada posició té el numero de pulsacions que s'ha
// de fer per a arribar a obtenir el nom de dum[i]. 
// Cost: Θ(n · x.length() · log(#simbols))
vector<nat> easy_dial::pulsacions(const vector<phone> &dum)
{
  vector<nat> res(dum.size(),0);
  for(nat i = 0; i < dum.size(); ++i){
    if(i == 0) res[i] = 0;
    else{
      nat cont = 1;
      string paraula = dum[i].nom();
      bool trobat = false;
      for(nat j = 0; j < paraula.size()+1 and !trobat; ++j){
        string aux = pulsacions_aux(paraula[j], j);
        if(aux == paraula){
          trobat = true;
          res[i] = cont;
        }
        else cont++;
        
      }
    }

  }

  return res;
} 

// Pre:  Cert
// Post: retorna un string que es el resultat de fer seguent passant-li el char x
// on x es cada char de la paraula de la qual volem saber la seva pulsacio
// Cost: Θ(x.length() · log(#simbols))
string easy_dial::pulsacions_aux(const char &x, nat &j)
{
  if(j == 0) inici();
  return seguent(x);
}


// Pre:  Cert
// Post: el vector v de phones ordenat de major frequència a menor
// Cost: Θ(n · log(n)), és a dir, cost quasi lineal.
void easy_dial::ordena(vector <phone>& v, vector <phone> v1, vector <phone> v2)
{
  int i = 0;
  int j = 0;
  int x = v1.size();
  int y = v2.size();
  int cont = 0;

  while(i < x and j < y){
      if (v1[i] > v2[j]) v[cont++] = v1[i++];
      else if (v2[j] > v1[i]) v[cont++] = v2[j++];
      else {
          v[cont++] = v1[i++];
          v[cont++] = v2[j++];
      }
  }

  while (i < x) v[cont++] = v1[i++];
  while (j < y) v[cont++] = v2[j++];
}

// Pre:  Cert
// Post: el vector v de phones ordenat de major frequència a menor
// Cost: Θ(n · log(n)), és a dir, cost quasi lineal.
void easy_dial::ordena_per_fusio(vector<phone> &v)
{

  if(v.size() <= 1) return;
  int a = v.size();
  int m = a/2;
  if (m < 1) return;
  int u = 0;

  vector <phone> v1(m), v2(a - m);
  for (int k = 0; k < m; ++k) v1[k] = v[k];
  for (int g = m; g < a; ++g) v2[u++] = v[g];

  ordena_per_fusio(v1);
  ordena_per_fusio(v2);

  ordena(v, v1, v2);
}

//OPERACIONS DE LA DOUBLE LIST
//CONSTRUCTOR BUIT
// Pre:  Cert
// Post: S'ha creat una doubleList buida
// Cost: Θ(1), és a dir, cost constant.
easy_dial::dobleList::dobleList() : _ini(nullptr), _fin(nullptr), _quants(0)
{

}

// Pre: Cert
// Post: s'ha modificar el paràmetre implícit que serà igual a L
// Cost: Θ(n), és a dir, cost lineal.
easy_dial::dobleList::dobleList(const dobleList &L)
{
  node_list* paux = L._ini;
  node_list* pant = nullptr;
  _ini = nullptr;
  _fin = nullptr;
  while(paux != nullptr){
    node_list* pnew = new node_list();
    pnew->_act = paux->_act;
    pnew->_v = paux->_v;
    pnew->_seg = nullptr;

    if(paux == L._ini) {
      _ini = pnew;
      pnew->_ant = nullptr;
    }
    else pnew->_ant = pant;

    _fin = pnew;
    pant = pnew;
    paux = paux->_seg;
  }
}

// Pre: Cert
// Post: s'ha modificar el paràmetre implícit que serà igual a L
// Cost: Θ(n), és a dir, cost lineal.
easy_dial::dobleList& easy_dial::dobleList::operator=(const dobleList &L)
{
  if(this != &L){
    node_list* paux = L._ini;
    node_list* pant = nullptr;
    _ini = nullptr;
    _fin = nullptr;
    while(paux != nullptr){
      node_list* pnew = new node_list();
      pnew->_act = paux->_act;
      pnew->_v = paux->_v;
      pnew->_seg = nullptr;

      if(paux == L._ini) {
        _ini = pnew;
        pnew->_ant = nullptr;
      }
      else pnew->_ant = pant;


        _fin = pnew;
      pant = pnew;
      paux = paux->_seg;
    }

  }
  return *this;
}

//DESTRUCTOR
// Pre:  Cert
// Post:  S'ha esborrat tota la dobleList
// Cost: Θ(n), és a dir, cost lineal.
easy_dial::dobleList::~dobleList()
{
  this->reinicia();
}

// Pre:  Cert
// Post:  S'ha insertat la visita visit a la dobleList
// Cost: Θ(1), és a dir, cost constant.
void easy_dial::dobleList::insertInList(node_tst* pactual, visita* visit)
{
  node_list *pnou = new node_list;
  pnou->_v = visit;
  pnou->_act = pactual;
  pnou->_seg = nullptr;
  pnou->_ant = _fin;
  ++_quants;
  if(_fin == nullptr) {
    _fin = pnou;
    _ini = pnou;
  }
  else {
    _fin->_seg = pnou;
    _fin = pnou;
  }
}

// Pre:  Cert
// Post:  S'ha esobrrat l'últim element de la dobleList
// Cost: Θ(1), és a dir, cost constant.
void easy_dial::dobleList::deleteInList()
{
  if(_fin == nullptr) return;
  else {  
    node_list *paux = _fin;
    if(paux == _ini){
      _fin = nullptr;
      _ini = nullptr;
    }
    else {
      _fin = paux->_ant;
      paux->_ant = nullptr;
      _fin->_seg = nullptr;
    }
      delete paux;
      --_quants;
  }
}

// Pre: Cert
// Post: S'ha esborrat tota la llista dobleList del paràmetre implicit
// Cost: Θ(n), és a dir, cost lineal.
void easy_dial::dobleList::reinicia()
{
  node_list* paux = _fin;
  while (paux != nullptr){
    node_list *p = paux;
    paux->_v->visitat = false;
    paux = paux->_ant;
    delete p;
    --_quants;
  }
  _fin = nullptr;
  _ini = nullptr;
}

// Pre:  Cert
// Post: Retorna un punter a l'últim element de la dobleList
// Cost: Θ(1), és a dir, cost constant.
typename easy_dial::dobleList::node_list* easy_dial::dobleList::getFin() const
{
  return _fin;
}

// Pre:  Cert
// Post: Retorna el nombre d'elements que hi ha a la dobleList
// Cost: Θ(1), és a dir, cost constant.                              
nat easy_dial::dobleList::getSize()
{
  return _quants;
}
