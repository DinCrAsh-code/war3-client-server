"""Inventory a committed src/ tree; classify directories, never infer coverage."""

import argparse
import collections
import csv
import io
from pathlib import Path
import subprocess


# A navigation label, not a claim that every function has the same responsibility.
AREAS = {
    "Agent": ("simulation", "handles,lifetime,relations,jass"),
    "Compress": ("resources", "compression"),
    "Config": ("resources", "configuration,map-overrides"),
    "Containers": ("resources", "shared-containers"),
    "Frame": ("presentation", "ui,events,layout"),
    "Game": ("startup", "context,resources,savegame"),
    "GameUI": ("presentation", "camera,selection,local-player"),
    "Input": ("presentation", "input,dispatch,cursor"),
    "Item": ("simulation", "items,abilities,inventory"),
    "Jass": ("jass", "vm,natives,handles,local-context"),
    "Math": ("simulation", "softfloat,geometry,shared-math"),
    "Misc": ("cross-cutting", "accessors,thunks,unclassified"),
    "Missile": ("simulation", "projectiles,effects"),
    "Net": ("network", "lockstep,commands,session,serialization"),
    "Os": ("resources", "platform,files,async-io"),
    "Pathfinding": ("simulation", "movement,pathing,collision"),
    "Player": ("simulation", "player-records,scores"),
    "Render": ("presentation", "graphics,resources,picking"),
    "Replay": ("network", "replay-diagnostics"),
    "Storm": ("resources", "shared-runtime,attachments"),
    "Sync": ("simulation", "time,checksum,rng,threads"),
    "Task": ("simulation", "tasks,scheduling"),
    "Unit": ("simulation", "units,visibility,selection,movement"),
    "Widget": ("simulation", "world-objects,visibility,fog,presentation"),
    "Window": ("presentation", "platform,input,window"),
}


def git(root, *args):
    return subprocess.check_output(["git", "-C", str(root), *args])


def render(root, ref):
    revision = git(root, "rev-parse", "--verify", ref + "^{commit}").decode().strip()
    entries = git(root, "ls-tree", "-r", "-z", revision, "--", "src/")
    rows = []
    counts = collections.Counter()
    formats = collections.Counter()
    for entry in entries.split(b"\0"):
        if not entry:
            continue
        metadata, raw_path = entry.split(b"\t", 1)
        _, kind, blob = metadata.decode().split()
        if kind != "blob":
            raise ValueError("Unsupported src entry: " + repr(entry))
        path = raw_path.decode("utf-8")
        parts = path.split("/")
        directory = parts[1] if len(parts) > 2 else "(root)"
        if directory == "(root)":
            subsystem, topics = "documentation", "source-navigation"
        else:
            # A newly added directory needs an explicit human classification.
            subsystem, topics = AREAS[directory]
        rows.append((path, directory, subsystem, topics, blob))
        counts[directory] += 1
        formats[Path(path).suffix] += 1
    if not rows:
        raise ValueError("The selected commit has no src/ files")

    stream = io.StringIO(newline="")
    writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
    writer.writerow(("path", "directory", "primary_area", "topics", "git_blob"))
    writer.writerows(rows)
    summary = [
        "# Каталог исходников",
        "",
        "Сгенерировано из Git, без исполнения или компиляции исходников.",
        f"Ревизия: `{revision}`.",
        f"Файлов: **{len(rows)}**; каталогов подсистем: **{sum(d != '(root)' for d in counts)}**.",
        "Форматы: " + ", ".join(f"`{ext}` — {n}" for ext, n in sorted(formats.items())) + ".",
        "",
        "[SOURCE_FILES.tsv](SOURCE_FILES.tsv) перечисляет каждый путь и Git blob.",
        "Классификация `primary_area`/`topics` наследуется от каталога и служит",
        "для навигации. Это не индивидуальная проверка файлов, число функций",
        "или оценка готовности. `cross-cutting` — служебная метка инвентаря;",
        "для FND по-прежнему выбирается один из subsystem в правилах реверса.",
        "Приоритеты конкретных цепочек: [карта src](../SRC_MAP.md).",
        "",
        "| Каталог | Файлов | Основная область | Пересечения |",
        "|---|---:|---|---|",
    ]
    for directory, count in sorted(counts.items()):
        area, topics = AREAS.get(directory, ("documentation", "source-navigation"))
        target = "../../../src" + ("/" + directory if directory != "(root)" else "")
        summary.append(f"| [{directory}]({target}) | {count} | {area} | {topics} |")
    summary.extend([
        "", "## Обновление", "",
        "Из корня репозитория, Python 3 и Git:", "", "```text",
        f"python research/reverse/tools/catalog_src.py --ref {revision}",
        f"python research/reverse/tools/catalog_src.py --ref {revision} --check",
        "```", "",
        "При новом импорте указать его commit; затем отдельно пересмотреть",
        "SRC_MAP, SOURCES и затронутые FND. Генератор не повышает evidence",
        "и не исправляет устаревшие комментарии в исходниках.", "",
    ])
    return {"SOURCE_FILES.tsv": stream.getvalue(), "README.md": "\n".join(summary)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ref", required=True, help="Commit containing the source import")
    parser.add_argument("--check", action="store_true", help="Compare without writing")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[3]
    output = root / "research/reverse/catalog"
    artifacts = render(root, args.ref)
    if args.check:
        for name, content in artifacts.items():
            path = output / name
            if not path.exists() or path.read_text(encoding="utf-8") != content:
                raise SystemExit("Catalog differs: " + name)
        print("Catalog matches the selected Git revision")
    else:
        output.mkdir(parents=True, exist_ok=True)
        for name, content in artifacts.items():
            (output / name).write_bytes(content.encode("utf-8"))
        print("Wrote source inventory and directory summary")


if __name__ == "__main__":
    main()
