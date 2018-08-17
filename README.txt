################################################################################
####                                                                        ####
####                                                                        ####
####                        READ ME Projet XIANGQI                          ####
####                        Sonia TIEO                                      ####
####                        Daniel DE MURAT                                 ####
####                        M1BIB                                           ####
####                                                                        ####
####                                                                        ####
################################################################################


Pour compiler et tester le projet, il faut effectuer dans le terminal les commandes suivantes:

1. $./compile
2. $./a.out         


Pour ce projet, nous avons choisi de coder un jeu d échecs chinois, le XIANGQI. Ce jeu se rapproche des échecs occidentaux mais il comprend certaines particularités. Chaque joueur dispose de deux tours, deux cavaliers, deux éléphants, deux gardes, d un roi, deux bombardes et de cinq pions. Le plateau est separe en deux par une riviere. 

Concernant l affichage, il a ete effectue en ASCII. Le mouvement des pieces s effectue par le biais de la fonction mouvement et des fonctions mouvtest qui testent si le mouvement est possible selon les règles du jeu.

La fonction afficher_message guide le joueur tout au long de la partie en lui indiquant si son mouvement est correct, s il a choisi le bon pion ou encore s il met en echec l adversaire. Si un joueur est mis en echec par son adversaire, il doit effectuer un coup pour défendre son roi et mettre fin à l echec.

La partie s arrete lorsque l un des deux joueurs se fait capturer son roi, ce qui ne fait pas partie des "vraies" regles du jeu. Nous avons ete contraint de prendre cette liberte car nous n avons pas reussi a implementer la fonction qui detectait l echec et mat d un joueur.








