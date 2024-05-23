---
hide:
    - toc
---

# Getting Started

To start working with zBassMusic, first we need to download the plugin.
We publish all releases on the [Releases](https://github.com/Silver-Ore-Team/zBassMusic/releases) page, so you can go
there and download zBassMusic.
Alternatively, below you should see the newest release:

<div class="gh-release">
    <p class="gh-release-title">&nbsp;</p>
    <p class="gh-release-changelog">&nbsp;</p>
    <ul class="gh-release-artifacts">
    </ul>
</div>

<script type="text/javascript">
    fetch('https://api.github.com/repos/Silver-Ore-Team/zBassMusic/releases')
        .then(d => d.json())
        .then(releases => {
            const release = releases[0];
            console.log(document.querySelector('.gh-release-title'));
            document.querySelector('.gh-release-title').textContent = `${release.name} (${release.published_at})`;

            document.querySelector('.gh-release-changelog').innerHTML = `<pre>${release.body}</pre>`;

            for (var i = 0; i < release.assets.length; i++) {
                const asset = release.assets[i];
                const li = document.createElement('li');
                li.innerHTML = `<a href="${asset.browser_download_url}">${asset.name}</a>`;
                document.querySelector('.gh-release-artifacts').appendChild(li);
            }
        });

</script>

There are different kinds of files for different use cases.

* `zBassMusic-{version}.zip`<br>ZIP archive with zBassMusic.dll and its dependencies. Use for loading the plugin
  directly from the disk.
* `zBassMusic-{version}.vdf`<br>Gothic VDF volume with zBassMusic.dll and its dependencies packed
  under `System/Autorun`.
  It's for loading the plugin using an external loader like Union.
* `zBassMusic-{version}-pdb.zip/vdf`<br>The same role as above but **-pdb** builds contain PDB debug symbols.
  You can use it if you are going to use a debugger and would like to see the source code instead of assembly for
  zBassMusic.

When you download the files, we can proceed to [loading the plugin](plugin-loading.md)