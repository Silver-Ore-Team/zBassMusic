# Contributing

We welcome contributions to the project with open arms.
This document will guide you through the process of contributing to the project.

## Prerequisites

* Fork the repository and setup project as described in the [Developer Guide](./index.md)
* Familiarize yourself with the [Style Guide](../style-guide/index.md)
* Familiarize yourself with
  the [Code of Conduct](https://github.com/Silver-Ore-Team/zBassMusic/blob/main/CODE_OF_CONDUCT.md) (or just be a nice
  guy, don't need to read it)

## What can I contribute?

We don't have any specific guidelines on what you can contribute, but the most welcome contributions are:

* Bug fixes
* Unsafe memory access fixes
* Code refactoring
* Documentation improvements

New features are also welcome, but please discuss them with the maintainers first.
You can find contact to maintainers on the [Home page](../../index.md).

## How to contribute?

1. Create a new branch from the `main` branch on your fork.
   We recommend naming the branch after the issue you are working on with a prefix `feature/`,
   `bugfix/`, `docs/`, etc. For example, `bugfix/unsafe_memory_access`.
2. Make your changes.
3. Test your changes.
4. Commit your changes.
5. Push your changes to your fork.
6. Create a pull request to the `main` branch of the main repository from your fork.

## Code Review

After you create a pull request, the maintainers will review your changes and provide feedback.
You may need to make additional changes before your pull request is accepted.

If you have any questions, feel free to ask them in the pull request or contact the maintainers directly.
In case you can't do the changes yourself,
let us know, and we will help you by providing guidance or doing the changes ourselves
(you will still be credited as a contributor).

If we don't accept your pull request, don't be discouraged. We will provide feedback on why it was rejected.

If you don't get any feedback for a long time, feel free to ping the maintainers in the pull request.

If we haven't heard from you for more than 21 days,
we may close the pull request or take it over and finish it ourselves.

## Branching & Merging Model

We use the Trunk-Based Development model for branching and Squash Merging for merging. This means that:

* You have to branch from the `main` branch.
* Your commits are squashed into a single commit before merging.
* You can't branch from other branches than `main` or you will get a merge conflict.
* Your branch is deleted after merging.

This model is chosen to keep the repository clean and make it easier to maintain.

In special cases,
we may create a separate trunk for some features if they are too big or too risky to be developed in the main trunk.
In this case the trunk will be named `trunk/FEATURE_NAME`
and will be merged back to the main trunk after the feature is finished.
All the rules from the Trunk-Based Development model still apply but to the feature trunk instead of the main trunk.

## License

By contributing to the project, you agree to license your contributions under the project's license.
You grant the project maintainers a non-exclusive, irrevocable, worldwide, royalty-free, sublicensable, transferable
license to use, reproduce, modify, prepare derivative works of, distribute, publicly perform, publicly display, and
otherwise exploit your contributions[^1].

[^1]: **Legal to human:** that just means you can't say "stop using my code" after you contributed it, we don't owe you,
anything, and we can do whatever we want with your code. You are stil the code author, though, and we can't
remove your name from the list of contributors or claim that we wrote the code you contributed.