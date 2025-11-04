<div align="center">
    <p>
        <img src="https://raw.githubusercontent.com/HichTala/draw2/refs/heads/main/figures/banner-draw.png">
    </p>


<div>

[![DRAW2 Workflow](https://github.com/HichTala/draw2-obsplugin/actions/workflows/push.yaml/badge.svg)](https://github.com/HichTala/draw2-obsplugin/actions/workflows/push.yaml)
[![Licence](https://img.shields.io/pypi/l/ultralytics)](LICENSE)
[![Github](https://img.shields.io/badge/-github-181717?logo=github&labelColor=555)](https://github.com/HichTala/draw2)
[![Twitter](https://badgen.net/badge/icon/twitter?icon=twitter&label)](https://twitter.com/tiazden)
[![HuggingFace Downloads](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fhuggingface.co%2Fapi%2Fmodels%2FHichTala%2Fdraw2&query=%24.downloads&logo=huggingface&label=downloads&color=%23FFD21E)](https://huggingface.co/HichTala/draw2)
[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)
[![WandB](https://img.shields.io/badge/visualize_in-W%26B-yellow?logo=weightsandbiases&color=%23FFBE00)](https://wandb.ai/hich_/draw)

[🇬🇧 English](README.md)

</div>

</div>

DRAW est le tout premier détecteur d'objets entraîné à détecter les cartes Yu-Gi-Oh! dans tous types d'images,
et en particulier dans les images de duels.

Ce projet est la partie plugin OBS du système DRAW 2. Il permet aux utilisateurs
d'intégrer de manière transparente le détecteur directement dans leurs streams ou leurs vidéos ;
et ceux **sans avoir de compétences techniques particulières**.
Le plugin peut afficher les cartes détectées en temps réel pour une expérience visuelle améliorée pour les spectateurs.

Ce projet est sous licence [GNU Affero General Public License v3.0](LICENCE) ; toutes les contributions sont les
bienvenues.

---

## <div align="center">📄Documentation</div>

### 🛠️ Installation

Suivez les instructions d'installation correspondant à votre système d'exploitation afin que tout fonctionne
correctement :

<details open>
<summary>🪟 Windows</summary>

1. Téléchargez le programme d'installation du plugin à partir de ce
   lien : [DRAW2 OBS Plugin Installer](https://github.com/HichTala/draw2-obsplugin/releases/download/0.1.0/draw2-obs-plugin-installer.exe)
2. Exécutez le programme d'installation et suivez les instructions à l'écran.
3. Une fois l'installation terminée, lancez OBS Studio. Si tout est correctement configuré, vous devriez voir dans le
   menu `Docks`
   une nouvelle option appelée `Draw 2`. Vous pouvez activer le dock et le placer où vous le souhaitez.
4. À ce stade, l'installation n'est pas encore terminée. Vous devez télécharger les poids du modèle DRAW 2.
   Fermez OBS Studio et ouvrez le dossier dans lequel le plugin est installé. Par défaut, il devrait se trouver dans :
   `C:\Program Files\draw2`. Ici, vous pouvez ouvrir le dossier `python`, cliquer avec le bouton droit de la souris
   n'importe où et sélectionner `Ouvrir dans le terminal`.
5. Dans le terminal, exécutez la commande suivante pour télécharger les poids du modèle :
   ```bash
   ./python.exe -c "import draw;draw.run()"
   ```
6. À cette étape, vous devriez voir apparaître des journaux, notamment des barres de progression indiquant le
   téléchargement des poids du modèle.
   Lorsque vous voyez
   ```bash
    Running Draw2 without OBS shared memory
    Waiting for OBS to start...
   ```
   le téléchargement est terminé, vous pouvez relancer OBS Studio.

</details>

<details>
<summary>🐧 Linux</summary>

À venir 👀

</details>

<details>
<summary>🍏 MacOS</summary>

Je ne connais pas suffisamment bien OBS sur macOS pour fournir un guide d'installation fiable.
Le plugin peut être compilé avec succès sur macOS, mais je ne l'ai pas testé de manière approfondie.
Si vous avez de l'expérience avec les plugins OBS sur macOS et que vous souhaitez contribuer à un guide d'installation,
n'hésitez pas à soumettre une demande d'extraction.
</details>

### 🚀 Utilisation

Une fois le plugin installé et les poids du modèle téléchargés, vous pouvez lancer OBS Studio.

1. Ouvrez le menu `Docks` et sélectionnez `Draw 2` pour activer le dock du plugin.
2. Dans le dock `Draw 2`, vous pouvez configurer les paramètres du plugin en cliquant sur l'icône en forme d'engrenage à
   côté du bouton `Start DRAW` :
    - **Sélectionner la liste de deck** : choisissez les deck lists qui contiennent les cartes que vous souhaitez
      détecter. 3 deck lists
      peuvent être gérées en même temps. Pour ajouter de nouvelles deck lists, vous pouvez cliquer sur le bouton
      `Ouvrir le dossier` et glisser-déposer
      vos fichiers deck lists (au format ydk) dans le dossier ouvert.
    - **Durée minimale hors écran** : durée minimale pendant laquelle une carte qui vient d'être détectée peut être
      affichée à nouveau.
    - **Durée minimale d'affichage** : durée minimale pendant laquelle une carte est affichée.
    - **Seuil de confidence** : définissez le niveau de confiance minimum pour la détection des cartes. Les détections
      inférieures à ce seuil seront ignorées.
3. Le plugin fournit une nouvelle source appelée `Affichage DRAW`. Vous pouvez l'ajouter à votre scène comme n'importe
   quelle autre source.
   Cette source affichera les cartes détectées à l'écran. Vous pouvez choisir la source/scène à partir de laquelle
   détecter les cartes.
4. Cliquez sur le bouton `Start DRAW` pour lancer le processus de détection. Le plugin commencera à détecter les cartes
   en temps réel
   et les affichera à l'écran à l'aide de la source `Draw Display`. Le plugin commence la détection dès que le bouton
   `Stop DRAW` s'affiche.
   Si vous ne le voyez pas, cela signifie qu'il y a eu un problème.
5. Dans le cas contraire, vous pouvez profiter du plugin !

---

## <div align="center">🔍Aperçu de la méthode</div>

Un blog medium expliquant le processus principal, de la collecte des données à la prédiction finale a été publié.
Vous pouvez le retrouver
[ici](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a).
Si vous avez des questions, n'hésitez pas à ouvrir une issue.

[![Medium](https://img.shields.io/badge/-Medium-12100E?style=flat&logo=medium&labelColor=555)](https://medium.com/@hich.tala.phd/how-i-trained-again-my-model-to-detect-and-recognise-a-wide-range-of-yu-gi-oh-cards-5c567a320b0a)

---

## <div align="center">💬Contact</div>

Vous pouvez me joindre sur Twitter [@tiazden](https://twitter.com/tiazden) ou par
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
