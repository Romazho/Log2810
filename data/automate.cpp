/****************************************************************************
 * Fichier:			automate.cpp
 * Auteurs: 		Kevin Ciliento, Gabriel Tagliabracci et Roman Zhornytskiy
 * Date:			20 mars 2019
 * Description:		Implémentation de la classe Automate
 ****************************************************************************/
#include "automate.h"
using namespace std;

void Automate::generateAutomate()
{
	//Acces aux rules
	vector<string> rules = door_->getRules();
	//Node * starter = new Node('S', false);
	Node * nullElem = new Node('E', true);	//est pas 'NE' car ca prend la derniere lettre...

	for (size_t i = 0; i < rules.size(); ++i) {
		//vérifier si le node existe deja sinon on crée un nouveau node.
		{//il serait plus intelligent de vérifier si S->_A par exemple et si oui alors on crée le node A
		//car si S ne mene pas à A, il est alors inutile de créer le node A. mais je ne pense pas que
		//ca change quelque chose à la fin...
		}
		auto it = find_if(nodeTree_.begin(), nodeTree_.end(), [rules, i](pair<Node*, vector<Edge*>> itNode) -> bool {return itNode.first->id_ == rules[i][0]; }); // Possible erreur ici, si oui mettre une pair
		Node* node = nullptr;

		if (it == nodeTree_.end()) // If no node has that name
		{
			node = new Node(rules[i][0], true);
		}

		else // If this node already exist
		{
			node = it->first;
		}
		
		if (rules[i].size() == 3) // _->
		{
			Edge * edge = new Edge(node, nullElem, '0'); // In ASCII 00 is null caracter

			nodeTree_.insert(node,edge);
		}
		
		else if (rules[i].size() == 4) // _->_
		{
			Edge * edge = new Edge(node, nullElem, rules[i][3]); // Verifier si final	//pas besoin car on est sur que c'est vrai tousjour.
			nodeTree_.insert(node,edge);
		}
		
		else // _->_ _
		{
			auto childIt = find_if(nodeTree_.begin(), nodeTree_.end(), [rules, i](pair<Node*,vector<Edge*>> itNode) {return itNode.first->id_ == rules[i][4]; });
			//Node* child = it->first; // marche pas 
			Node* child = nullptr;
			
			if (childIt == nodeTree_.end()) 
			{
				child = new Node(rules[i][4], true);
			}

			else
			{
				child = childIt->first;
			}
			
			Edge * edge = new Edge(node, child, rules[i][3]); // Verifier si final	//pas besoin car on est sur que c'est vrai tousjour.
			//verifier si node a un child
			//for(int i=0;i<node->childs_.size();i++)
			//{
			//	if (node->childs_[i]->id_ != 'E' ) {

			//	}
			//}
			
			node->isFinal_ = false;	//pas tousjour vrai, car S-> peut etre avant et ensuite on fait ca ce qui se contredit...
			node->childs_.push_back(child);
			nodeTree_.insert(node,edge);
		
		}
	}

	//remplissage de toValidate_
	for (int i = 0; i < door_->getPasswords().size(); i++) {
		
		toValidate_.push_back(door_->getPasswords()[i]);
	}

	validatePasswords();
}

void Automate::validatePasswords() {
	if(door_->getDoorName() != "Boss")
	{
		for (int i = 0; i < toValidate_.size(); i++) {

			validatePassword(toValidate_[i]);

		}
	}
	else
	{
		validatePassword(door_->getPasswords().front());
	}

}

/**
 * \brief Validate an confirmed password
 * \param password found by the Automate
 */
//on valide un password
void Automate::validatePassword(const string& password)
{
	//car password est deja validé dans door.
	if (password == "")
		return;

	//on commence tousjour de S
	vector<Edge*> startEdges = nodeTree_.at('S');
	char etatActuel = 'S';
	char etatPrecedent = 'S';

	int longueurChemin = 0;

	//fonction qui parcourt le chemin qui prend en paramètre la première lettre
	//et qui retourne un bool
	//on parcourt le chemin = password
	for (int i = 0; i < password.size(); i++) {

		bool trouvee = trouverLettre(password[i], longueurChemin, startEdges, etatActuel, etatPrecedent);

		//si on a trouvé la lettre alors on cherche la suivante.
		if (trouvee == true) {
			//besoin de donner l'id du prochain état ex:A
			if (etatActuel != 'E') {
				startEdges = nodeTree_.at(etatActuel);
			}
		}
		//si on a pas trouvé alors il est inutile de continuer.
		else {
			break;
		}

	}


	//on valide le password si tout est correct
	//virifier si on se trouve à un état final

	if ( (longueurChemin == password.size())  && ((etatActuel == 'E') || (etatActuel == 'S')) ) {

		if (etatActuel == 'S') {
			vector<Edge*> Edges = nodeTree_.at('S');
			auto it = find_if(Edges.begin(), Edges.end(), [](Edge* edge) {return edge->value_ == '0'; });
			if (it != Edges.end()) {
				door_->getDoorName() == "Boss" ? door_->validateBossDoor() : door_->validate(password);
				lastNode_ = etatActuel;
			}

		}
		else {
			
			//retourner le node avant 'E'
			door_->getDoorName() == "Boss" ? door_->validateBossDoor() : door_->validate(password);
			lastNode_ = etatPrecedent;
		
		}
	}

	
}


bool Automate::trouverLettre(const char& lettre, int& longueurChemin, vector<Edge*>& startEdges, char& etatActuel, char& etatPrecedent) {

	//on parcourt tous les edges du starter
	for (int i = 0; i < startEdges.size(); i++) {

		//si on a trouvé le bon chemin
		//verifier si la destination est differente de l'état actuelle
		if ( (startEdges[i]->value_ == lettre) && (startEdges[i]->src_->id_ == etatActuel) ) {

			etatPrecedent = startEdges[i]->src_->id_;
			etatActuel = startEdges[i]->dest_->id_;
			longueurChemin++;
			return true;
		}
	}
	return false;
}
