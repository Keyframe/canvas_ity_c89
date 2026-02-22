# canvas-ity-c89 — TOT Configuration

**Description**: A tiny, single-header <canvas>-like 2D rasterizer for C89 based on canvas_ity (C++)

## Commands

| Operation | Command |
|-----------|---------|
| **Test** | `make test` |
| **Lint** | `gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -fsyntax-only src/canvas_ity.h` |
| **Analyze** | `make analyze` |
| **Build** | `make` |
| **Format** | `echo 'no format tool available'` |

## Language

- **Primary**: C (C89 target)

## Integrations

- **GitHub remote**: https://github.com/Keyframe/canvas_ity
- **GitHub CLI (gh)**: available, authenticated as Keyframe
- **Linear**: no
- **Linear project prefix**: _N/A_

## Hooks

- **Original hooks path**: _none_
- **TOT hooks path**: `.tot-hooks/`
- **Hook chaining**: Original hooks run first, then TOT enforcement

## Settings

- **One active WI at a time**: yes
- **Branch pattern**: `tot/<type>/WI-NNN-description`
- **Commit format**: `WI-NNN type(scope): description` (Conventional Commits v1.0.0)
- **Push on commit**: yes
- **Scope enforcement**: strict (block on violation)
- **Merge strategy**: `--no-ff` (always create merge commit)
- **PR workflow**: draft on first push, ready on tot-finish
