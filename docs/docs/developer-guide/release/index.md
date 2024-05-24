# Release

Release of a new version can be performed only by the project's maintainers after accumulating enough changes.

## Versioning

We follow [Semantic Versioning 2.0.0](https://semver.org/).

As long as we stay on MAJOR = 0, the rules of version bumping are liberal, and we can bump MINOR only on finalized
milestone features, while using PATCH for small features and bugfixes.

## Release Build

To create a Release Build, the maintainer creates a tag on `main` branch with the version and pushes it.

```bash
git tag v1.2.3
git push --tags
```

GitHub Actions workflow will start and create a draft release on GitHub. The maintainer reviews the release
and publishes it manually.

### Release Workflow Failure

If the workflow fails, we have to fix it and clean up unused tags.

1. Remove tag from GitHub using the UI.
2. Remove local tag: `git tag -d v1.2.3`
3. Create tag again: `git tag v1.2.3`
4. Push tags again: `git push --tags`

## Non-Release Builds

It's also possible to create GitHub releases that don't count as an official Release.
Such a build must have a version suffix like `v1.2.3-rc1`.
To create a Non-Release Build, follow the same rules as with Release Build but create a tag with a suffix.  