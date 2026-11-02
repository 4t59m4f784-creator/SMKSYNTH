# Publication preparation verification

- Local HTML and JS dependency paths: passed, including the backup-b1 client used by B2.
- Project-subpath paths: passed; runtime tools use relative paths.
- B2 firmware SHA-256: unchanged from tested 19615.
- Approved P16.2 simulator WASM SHA-256: unchanged.
- All firmware source bytes: identical to B2 source snapshot.
- Existing B2 installer validation test: passed (package pins, origin restrictions and resume).
- Source archive: firmware, loader, build tools and license present; no device .smksynth bank files.
- Focused text scan found no private-key or recognized access-token patterns. This is a scan, not an exhaustive security audit.
- Browser preview: landing page rendered and simulator started in interface-only mode at the HTTP preview origin. The native Sites service confirmed successful owner-private deployment. The live cloud browser reached the sign-in gate, so authenticated HTTPS/audio/package UI verification was not performed in that browser; local package pins were checked by the existing installer test.
- The browser environment cannot validate physical USB transfers. Previous user validation of B2 remains the physical export/restore evidence.
- GitHub Actions workflow has not run on GitHub: the repository is not yet created.
