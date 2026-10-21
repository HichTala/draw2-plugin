<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/banner.png" alt="DRAW Banner">
    </p>


<div>

[![Web Interface](https://img.shields.io/badge/🌐_Web_Interface-Try_It_Now-blue)](https://hichtala.github.io/draw2)

[![DRAW2 Workflow](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-plugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](../LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://img.shields.io/badge/-twitter-000?logo=x&labelColor=555)](https://twitter.com/hichtala)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇬🇧 English](../README.md) | [🇧🇷 Português](README_pt-br.md) | [🇯🇵 日本語](README_jp.md) | [🇪🇸 Español](README_es.md)

</div>

</div>

DRAW est le tout premier détecteur d'objets entraîné à détecter les cartes Yu-Gi-Oh! dans tous types d'images,
et en particulier dans les images de duels.

Ce projet est la partie plugin du système DRAW 2. Il permet aux utilisateurs
d'intégrer de manière transparente le détecteur directement dans leurs streams ou leurs vidéos ;
et ceux **sans avoir de compétences techniques particulières**.
Le plugin peut afficher les cartes détectées en temps réel pour une expérience visuelle améliorée pour les spectateurs.

Ce projet est sous licence [GNU Affero General Public License v3.0](LICENCE) ; toutes les contributions sont les
bienvenues.

---
## <div align="center">📰 News</div>

> 🃏 **Dernière extension supportée:** `CORI` --- mise à jour le `13-06-2026`  
> 🔧 **Dernière version:** `0.2.1-beta` --- mise à jour le `01-06-2026`

<table>
  <tr>
    <th>Date</th>
    <th>Type</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>13-06-2026</b></td>
    <td>🃏 Pool de cartes</td>
    <td>Mise à jour du pool de cartes --- prend désormais en charge les cartes jusqu'à <i>Les Origines du Chaos</i></td>
  </tr>
  <tr>
    <td><b>01-06-2026</b></td>
    <td>🔧 Version de l'app</td>
    <td>Dernière version 0.2.1-beta --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.2.1">voir les notes de maj</a></td>
  </tr>
  <tr>
    <td><b>18-05-2026</b></td>
    <td>🃏 Pool de cartes</td>
    <td>Mise à jour du pool de cartes --- prend désormais en charge les cartes jusqu'à <i>Territoire Embrasé</i></td>
  </tr>
  <tr>
    <td><b>06-04-2026</b></td>
    <td>🃏 Pool de cartes</td>
    <td>Mise à jour du pool de cartes --- prend désormais en charge les cartes jusqu'à <i>Le Labyrinthe des Morts</i></td>
  </tr>
  <tr>
    <td><b>06-04-2026</b></td>
    <td>🔧 Version de l'app</td>
    <td>Dernière version 0.2.0-beta --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.2.0">voir les notes de maj</a></td>
  </tr>
  <tr>
    <td><b>09-03-2025</b></td>
    <td>🔧 Version de l'app</td>
    <td>Dernière version 0.1.5-alpha --- <a href="https://github.com/HichTala/draw2-plugin/releases/tag/0.1.5">voir les notes de maj</a></td>
  </tr>
  <tr>
    <td><b>24-08-2025</b></td>
    <td>🃏 Pool de cartes</td>
    <td>Mise à jour du pool de cartes --- prend désormais en charge les cartes jusqu'à <i>Les Chasseurs de Justice</i></td>
  </tr>
</table>

---

## <div align="center">📄Documentation</div>

### 🛠️ Installation

Suivez les instructions d'installation correspondant à votre système d'exploitation afin que tout fonctionne
correctement :

<details open>
<summary>🪟 Windows</summary>

1. Téléchargez le programme d'installation du plugin à partir de ce
   lien : [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-installer.exe)
2. Exécutez le programme d'installation et suivez les instructions à l'écran.
3. Une fois l'installation terminée, lancez OBS Studio. Si tout est correctement configuré, vous devriez voir dans le
   menu `Docks`
   une nouvelle option appelée `Draw 2`. Vous pouvez activer le dock et le placer où vous le souhaitez.

   Le téléchargement est terminé ! Bonne détection à tous!

</details>

<details>
<summary>🐧 Linux</summary>

1. Téléchargez le programme d'installation du plugin à partir de ce lien :
   [DRAW2 Plugin Installer](https://github.com/HichTala/draw2-plugin/releases/download/0.2.1/draw2-plugin-0.2.1-x86_64-linux-gnu.deb)

2. Exécutez le programme d'installation en double-cliquant dessus, puis cliquez sur installer _OU_ en lançant la
   commande :

   ```shell
   sudo apt install ./draw2-plugin-0.2.1-x86_64-linux-gnu.deb
   ```

3. Une fois l'installation terminée, lancez OBS Studio. Si tout est correctement configuré, vous devriez voir dans le
   menu `Docks` une nouvelle option appelée `Draw 2`. Vous pouvez activer le dock et le placer où vous le souhaitez.
   L'installation n'est pas encore terminée : le plugin est installé, mais vous devez encore installer le backend
   Python. Fermez OBS et suivez les étapes suivantes.

4. Si vous avez déjà une installation Python que vous souhaitez utiliser, vous pouvez passer cette étape. Toute
   installation Python avec le paquet `draw` installé peut fonctionner. Nous utiliserons un CPython autonome. Une
   version relocalisable de [python-build-standalone](https://github.com/astral-sh/python-build-standalone) :

   ```shell
   # choisissez la version install_only correspondant à votre architecture (x86_64 pour les CPU Intel et AMD)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-x86_64-unknown-linux-gnu-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. Installez le backend `draw` (assurez-vous que [git](https://git-scm.com/install/linux) est installé) :

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Dans les paramètres de Draw 2, définissez **Sélectionner l'installation Python** sur le dossier préfixe
   (celui qui contient `bin/` et `lib/`), par ex. `~/.draw2-runtime/python`.
   Structure attendue :

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

L'installation est terminée, bonne détection à tous !

</details>

<details>
<summary>🍏 MacOS</summary>

1. Téléchargez le programme d'installation du plugin à partir de ce lien :
   [DRAW2 Plugin Installer]() (la version macOS n'a pas encore été publiée, nous y travaillons et elle sortira
   bientôt ; en attendant, vous pouvez utiliser le plugin en le compilant depuis les sources, voir la section
   [Building from source](../README.md) du README en anglais)

2. Exécutez le programme d'installation en double-cliquant dessus. Une fenêtre devrait vous indiquer qu'Apple n'a pas
   pu vérifier le plugin : fermez-la (`Done`), puis ouvrez les réglages système et cherchez
   `Confidentialité et sécurité` (`Privacy & Security`). Faites défiler jusqu'à voir
   `"draw2-plugin....pkg" was blocked to protect your Mac` (le message peut s'afficher dans votre langue), cliquez sur
   `Ouvrir quand même` (`Open Anyway`), puis de nouveau sur `Ouvrir quand même` et suivez les instructions à l'écran.

3. Une fois l'installation terminée, lancez OBS Studio. Si tout est correctement configuré, vous devriez voir dans le
   menu `Docks` une nouvelle option appelée `Draw 2`. Vous pouvez activer le dock et le placer où vous le souhaitez.
   L'installation n'est pas encore terminée : le plugin est installé, mais vous devez encore installer le backend
   Python. Fermez OBS et suivez les étapes suivantes.

4. Si vous avez déjà une installation Python que vous souhaitez utiliser, vous pouvez passer cette étape. Toute
   installation Python avec le paquet `draw` installé peut fonctionner. Nous utiliserons un CPython autonome. Une
   version relocalisable de [python-build-standalone](https://github.com/astral-sh/python-build-standalone) :

   ```shell
   # choisissez la version install_only correspondant à votre architecture (aarch64 pour Apple Silicon)
   curl -fL -o python.tar.gz https://github.com/astral-sh/python-build-standalone/releases/download/20261009/cpython-3.13.16+20261009-aarch64-apple-darwin-install_only.tar.gz
   mkdir -p ~/.draw2-runtime && tar -xzf python.tar.gz -C ~/.draw2-runtime
   ```

5. Installez le backend `draw` (assurez-vous que [git](https://git-scm.com/install/mac) est installé) :

   ```shell
   ~/.draw2-runtime/python/bin/python -m pip install "git+https://github.com/HichTala/draw2@obs-plugin"
   ```

6. Dans les paramètres de Draw 2, définissez **Sélectionner l'installation Python** sur le dossier préfixe
   (celui qui contient `bin/` et `lib/`), par ex. `~/.draw2-runtime/python`.
   Structure attendue :

   ```text
   <prefix>/bin/python
   <prefix>/lib/python3.13/site-packages/draw
   ```

L'installation est terminée, bonne détection à tous !

</details>

### 🚀 Utilisation

Une fois le plugin installé et les poids du modèle téléchargés, vous pouvez lancer OBS Studio.

1. Ouvrez le menu `Docks` et sélectionnez `Draw 2` pour activer le dock du plugin.
2. Dans le dock `Draw 2`, vous pouvez configurer les paramètres du plugin en cliquant sur l'icône en forme d'engrenage à
   côté du bouton `Start DRAW` :
   - **Sélectionner l'installation Python** : chemin vers le préfixe Python sur lequel le backend `draw` est installé
     (le dossier contenant `bin/` et `lib/`). Il doit s'agir d'une installation Python complète, pas d'un
     virtualenv.
   - **Sélectionner les listes de deck** : choisissez les deck lists qui contiennent les cartes que vous souhaitez
     détecter — jusqu'à 3. Pour ajouter des deck lists, cliquez sur `Open Folder` et déposez vos fichiers `.ydk` dans le dossier ouvert.
   - **Durée minimale hors écran** : durée minimale pendant laquelle une carte qui vient d'être détectée peut être
     affichée à nouveau.
   - **Durée minimale d'affichage** : durée minimale pendant laquelle une carte est affichée.
   - **Seuil de confidence** : définissez le niveau de confiance minimum pour la détection des cartes. Les détections
     inférieures à ce seuil seront ignorées.
   - **Fonctionnalités avancées** (désactivées par défaut) : deux réglages optionnels de l'entrée du détecteur qui
     n'affectent que ce que voit le détecteur, pas votre sortie en direct. Activez-les ici, puis configurez les
     valeurs sur la source `Draw Display` :
     - **Activer deux joueurs** — ajoute un ensemble de deck lists **Joueur 2** et lance un **détecteur distinct par joueur**, permettant de traiter deux joueurs simultanément. Laissez cette option désactivée pour une configuration à un seul joueur.
     - **Activer le recadrage de l'entrée du détecteur** — ajoute des champs **Rogner (Gauche/Haut/Droite/Bas, px)** à la source,
       pour cibler la zone où les cartes sont posées.
     - Lorsque qu'elle est activée, l'option **Aperçu de l'entrée du détecteur** apparait:
       activez-la pour que la source affiche l'image recadrée/pivotée qu'elle envoie au détecteur (afin de
       régler le recadrage directement dans l'aperçu de la source), puis désactivez-le pour revenir à
       l'affichage des cartes détectées.
   - **Enable debug logging** (désactivé par défaut) : affiche des diagnostics détaillés — le journal du dock
     montre le lancement exact du backend (chemins de deck résolus, canal, seuils) et chaque ligne du backend
     sans filtrage, et le journal OBS reçoit les détails de capture (source d'entrée, dimensions,
     recadrage). Pratique quand la détection ne se comporte pas comme prévu.
   - **Activer la decklist distante** — permet à chaque joueur de charger son deck depuis une URL HTTP(S) ou un code YDKE au lieu d'un fichier `.ydk` local ; voir la note sous la source `Affichage Draw`.
3. Le plugin fournit une nouvelle source appelée `Affichage DRAW`. Vous pouvez l'ajouter à votre scène comme n'importe
   quelle autre source.
   Cette source affichera les cartes détectées à l'écran. Vous pouvez choisir la source/scène à partir de laquelle
   détecter les cartes. Avec le mode deux joueurs activé, utilisez la propriété **Détecteur / Joueur** de la source pour choisir de quel détecteur elle
   lit (**Joueur 1** ou **Joueur 2**) ; ajoutez un `Affichage Draw` par joueur pour afficher les deux simultanément.

   > 💡 Le plugin peut également charger une deck list depuis une **URL HTTP(S)** ou un **code YDKE** (`ydke://…`)
   > plutôt qu'un fichier `.ydk` local — pratique lorsque votre liste de cartes est servie par une API distante ou un
   > système de gestion de tournoi, ou partagée sous forme de lien YDKE. Cette fonctionnalité est **désactivée par
   > défaut** — activez **Activer la decklist distante** dans les paramètres de Draw 2 (onglet *Fonctionnalités*) ; les
   > sources se configurent ensuite dans un onglet dédié **Decklist**.
   >
   > Deux modes sont disponibles une fois activé :
   > - **Importer un deck depuis une URL / YDKE…** (bouton dans l'onglet **Decklist**) — récupère l'URL (ou décode le code
   >   YDKE) une seule fois et enregistre le résultat comme un fichier `.ydk` normal dans le dossier des deck lists, vous
   >   pouvez ainsi l'utiliser comme un deck fichier ordinaire.
   > - **Source de deck par joueur** — chaque joueur (Joueur 1, plus Joueur 2 en mode deux joueurs) choisit sa propre
   >   source : **Fichiers de deck** (les sélecteurs habituels), **URL** (récupérée en direct au clic sur Lancer DRAW) ou
   >   **Code YDKE**. Les sources peuvent être combinées, p. ex. Joueur 1 depuis une URL et Joueur 2 depuis des fichiers.
   >   En cas d'échec URL/YDKE, ce joueur démarre simplement sans filtre de deck.
   >
   > Formats de réponse acceptés : un code `ydke://` ; tableau JSON d'ID de carte (passcodes) ; objet JSON
   > `{ "main": [...], "extra": [...], "side": [...] }` ; texte `.ydk` brut ; ou texte brut contenant des ID numériques.
   >
   > Un en-tête d'authentification optionnel (nom + valeur, p. ex. `Authorization` / `Bearer …`) peut être configuré
   > et est envoyé avec chaque requête.
   >
   > ⚠️ Les URLs et la valeur de l'en-tête sont stockées en texte brut dans QSettings, comme tous les autres paramètres
   > du plugin — évitez d'y placer des secrets à longue durée de vie.
4. Cliquez sur le bouton `Lancer DRAW` pour lancer le processus de détection. Le plugin commencera à détecter les cartes
   en temps réel
   et les affichera à l'écran à l'aide de la source `Affichage Draw`. Le plugin commence la détection dès que le bouton
   `Arrêter DRAW` s'affiche.
   Si vous ne le voyez pas, cela signifie qu'il y a eu un problème.
5. Dans le cas contraire, vous pouvez profiter du plugin !

Voici un petit aperçu :)
<div align="center">
    <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/docs/assets/overview.gif" width="960" height="540" />
</div>

### ⚙️ Compilation depuis le code source

> Les instructions pour compiler le plugin depuis le code source (Windows, Linux et macOS), y compris la configuration
> du backend Python, sont uniquement disponibles en anglais. Consultez la section **Building from source** du
> [README en anglais](../README.md).

---

## <div align="center">🔍Aperçu de la méthode</div>

Un blog medium expliquant le processus principal, de la collecte des données à la prédiction finale a été publié.
Vous pouvez le retrouver
[ici](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a).
Si vous avez des questions, n'hésitez pas à ouvrir une issue.

[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)

---

## <div align="center">💬Contact</div>

Vous pouvez me joindre sur Twitter [@hichtala](https://twitter.com/hichtala) ou par
mail [hich.tala.phd@gmail.com](mailto:hich.tala.phd@gmail.com).

---

## <div align="center">⭐Historique des Stars</div>

<a href="https://www.star-history.com/#HichTala/draw2&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=HichTala/draw2&type=date&legend=top-left" />
 </picture>
</a>