import os


# walk over files and check for "// Copyright 2024 Phillip Weinberg"


def check_files(Copyright="// Copyright 2024 Phillip Weinberg"):
    dirs = ["src", "include", "test"]
    for d in dirs:
        for root, dirs, files in os.walk(d):
            for file in files:
                if not file.endswith(".cpp") and not file.endswith(".hpp"):
                    continue

                with open(os.path.join(root, file), "r") as f:
                    try:
                        line_0 = next(f)
                    except StopIteration:
                        continue

                if line_0 == Copyright:
                    continue

                with open(os.path.join(root, file), "r") as f:
                    lines = f.readlines()

                with open(os.path.join(root, file), "w") as f:
                    f.write(f"{Copyright}\n")
                    for line in lines:
                        f.write(line)


if __name__ == "__main__":
    check_files()
