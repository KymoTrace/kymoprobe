# KymoProbe and KymoCore versions

KymoProbe release tags use **exactly the KymoCore release version**:
`vX.Y.Z`. The tagged Probe commit must pin `lib/KymoCore` to the official Core
tag `vX.Y.Z`. All examples consume that submodule; no sibling development
checkout or floating registry dependency is allowed.

## Existing baseline

`v7.3.0` preserves the **unchanged** Probe commit
`4ad2ba343d47b11c9f7ffdde6da70f3d4a1628cd`, before the example refresh.
Its Core submodule is `d08fe07c00d58c94f37400b2737f560c3037f3b9` (`v7.3.0`).
The baseline still contains three historical example references to a sibling
Core checkout; these are corrected in the subsequent development changes.

Tags are immutable. Improvements made afterwards remain on `develop` until
the next matching Core release. A Probe tag identifies a reproducible release
snapshot, not every intermediate example revision. Never move `v7.3.0` to
the refreshed examples or invent an unrelated Probe version number.

## Future release procedure

1. Publish the KymoCore release in its own repository first.
2. Fetch its official tag in `lib/KymoCore` and check it out detached.
3. Adapt and verify Probe examples, update version references in documentation,
   then commit the submodule pointer and the Probe changes together.
4. Run `python tools/check_core_version.py --tag vX.Y.Z`. This checks the
   committed gitlink, actual Core checkout, Core release tag, package versions,
   clean detached consumer checkout and all PlatformIO example dependencies.
5. Run `python tools/test_native.py --app ../KymoStudio` and affected firmware
   builds. Native testing includes the full modular integration example.
6. Create the annotated Probe tag with the same version and publish it:

   ```sh
   git tag -a vX.Y.Z -m "KymoProbe examples for KymoCore vX.Y.Z"
   git push origin develop
   git push origin refs/tags/vX.Y.Z
   ```

Replace `X.Y.Z` with the actual Core release. A pre-existing tag must cause the
procedure to stop, never use `-f`. Tag CI validates the same version relationship;
normal branch CI checks the pinned dependency without demanding a new tag.
The new checks apply to commits containing them; the preserved baseline is not
rewritten to add CI or fix historical files.
