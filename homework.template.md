# Assignment — session-XX
**Deadline: YYYY-MM-DD HH:MM:SS**

> **How to use this template:**
> - Remove all `>` instruction lines before pushing
> - Line 2 MUST contain deadline in format: `**Deadline: YYYY-MM-DD HH:MM:SS**` (e.g. `**Deadline: 2026-06-14 23:59:00**`)
> - Sections marked `(optional)` can be omitted if not needed
> - The `[build]` / `[review-only]` tag in each Exercise heading is **required**

---

## Exercise_1 [build]

> Tag `[build]`: the system will compile and run the program — a Makefile is required.
> Tag `[review-only]`: static code review only — no Makefile needed (used for Yocto, Device Tree, MCU, etc.).

### Problem Statement

<!-- Describe the exercise requirements. Be as specific as possible. -->

Write a C program that ...

### Design Hints (optional)

<!-- Suggest structs, enums, or typedefs to guide students. Remove this section if you want students to design their own. -->

```c
typedef struct {
    int fd;
    char name[64];
} my_struct_t;
```

### Suggested Approach (optional)

<!-- Outline the main processing flow so students don't get lost. Remove if you want students to figure it out themselves. -->

```
1. Initialize ...
2. Main loop: read data → process → write result
3. Clean up resources before exiting
```

### Expected Output (optional)

<!-- Describe the expected program output. Use when automatic output checking is needed. -->

When run with input `...`, the output must be:

```
expected output line 1
expected output line 2
```

Exit code: `0` on success, non-zero on error.

### Submission

```
Exercise_1/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 [review-only]

### Problem Statement

Write a Device Tree overlay for ...

### Submission

```
Exercise_2/
└── my-overlay.dts    (required)
```

---

## Exercise_3 [build]

### Problem Statement

...

### Submission

```
Exercise_3/
├── main.c
└── Makefile
```
