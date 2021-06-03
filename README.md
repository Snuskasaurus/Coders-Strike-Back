COMPTE RENDU

Profil GitHub :
https://github.com/Snuskasaurus/Coders-Strike-Back

Profil CodinGame :
https://www.codingame.com/profile/9b0beea058ab2313cf71a078ee92d61d5633634

Rang obtenu : 
Gold 164 / 6451
Rang bois et bronze
J’ai en premier temps, dès le bois, structuré le projet en créant la classe des pods et des checkpoints pour préparer le bronze.

La première mécanique est le boost : Son implémentation a été plutôt simple, j’ai simplement: 
Le joueur observe si le pod ennemi n'était pas devant et à une distance trop proche de lui.
Le joueur observe si la distance qui le sépare de son checkpoint est suffisante.

Pendant le développement du boost, j’ai également développé la class Vector2, qui me permet de stocker les positions et vitesses des pods/checkpoints.

Ces ajouts m’ont fait grimper au classement bronze 764 sur 30505

Ajouter le boost n'était donc pas suffisant, il était temps de s’occuper de la trajectoire du pod pendant les virages en ajoutant quelques règles:
Le joueur ralentit progressivement quand il s’approche du prochain checkpoint.
Le joueur ralentit progressivement quand il tourne vers son prochain checkpoint.
Le joueur se dirige maintenant vers une nouvelle position qui prend en compte sa vitesse et la position de son prochain checkpoint.

Tout cela m’a fait monter directement en or.
Rang or
La manière dont sont maintenant géré les inputs m’on fait faire quelques changements. Heureusement, grâce à mon architecture, cela ne m’a pas pris énormément de temps.

Cela m’a permis d'être classé 1519ème sur 6439

J’ai effectué quelques réglages de bugs et quelques ajustements. Le second joueur utilise maintenant son boost sur le checkpoint qui possède la plus grande distance tandis que le deuxième boost dès qu’il le peut.
Cela m’a permis de gagner très peu de places.

Faisant quelques recherches et en analysant mes parties, j’ai compris qu’une approche standard allait me prendre beaucoup de temps et n’allait peut-être même pas fonctionner due à la nature chaotique des courses. J’ai donc décidé d’apprendre à utiliser l'algorithme génétique. 

Cela à été très complexe mais après quelques jours j’ai réussi à implémenter un prototype de cette approche sur la poussée des pods.

J’ai ensuite ajouté un gène pour gérer la rotation des pods, 
Cela m’a permis d'être classé 270ème sur 6439.

Cependant, je ne simule pas les collisions entre checkpoints ou entre pods, et cela engendre certains mauvais gènes de pouvoir être gardés.

En ajoutant les collisions dans la simulation, je me suis rendu compte que le nombre de mutations générées avait beaucoup diminué, les gènes étaient alors de "mauvaise qualité”. J’ai donc eu l’idée de rendre idiot mon deuxième pod. Cela m’a permis de doubler le montant de mutations à chaque frame. J’ai donc rajouté une gêne pour utiliser le boost.

Cette dernière milestone m’a donc permis d'être classé 164ème sur 6451 dans la league or.
Résumé
Cette aventure a été très enrichissante, mais accompagnée de beaucoup de frustrations, provenant de : l'absence de breakpoint sur codinGame, l’utilisation de l'algorithme génétique que je souhaitais éviter, et la mauvaise documentation sur l'implémentation des collisions.

De nombreuses choses pourraient être améliorées pour passer en légende, je pourrais par exemple faire en sorte que le second pod embête les ennemis et créer une gêne pour l’utilisation du bouclier.
