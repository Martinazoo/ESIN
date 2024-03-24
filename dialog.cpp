#include "dialog.hpp"

// Pre:  Cert
/* Post: Retorna en el vector answers els resultats obtinguts al processar
els successius caràcters de l'string input, i en numtelf retorna 
el número de telèfon corresponent a l'últim nom obtingut o un 0 si
no existeix aquest nom de telèfon. Si durant el processament de la
seqüència de tecles representada en input es produís un error
llavors a answers es registra el missatge d'error associat a
l'excepció, numtelf és un 0 i finalitza el procés. */
void dialog::dialog(easy_dial& easy, const string& input, 
              vector<string>& answers, nat& numtelf) throw()
              {
                answers.push_back(easy.inici());
                    try {
                        for(nat i = 0; i < input.size(); i++){
                            if(input[i] == phone::ENDCHAR) answers.push_back(easy.seguent(phone::ENDPREF));
                            else if(input[i] == phone::DELETECHAR) answers.push_back(easy.anterior());
                            else answers.push_back(easy.seguent(input[i]));
                        }

                        numtelf = easy.num_telf();
                    }
                    catch(error e){
                        numtelf = 0;
                        answers.push_back(e.mensaje());
                    }
              }