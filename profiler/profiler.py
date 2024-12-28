import subprocess
import json
from enum import StrEnum
from dataclasses import dataclass
import matplotlib.pyplot as plt

import catppuccin

class ProcessErrorsType(StrEnum):
    NONE_ZERO_RETURN_CODE = "none_zero_return_code"
    INVALID_JSON = "invalid_json"
    MISSING_FIELDS = "missing_fields"

@dataclass
class ProcessError:
    type: ProcessErrorsType
    payload: str

@dataclass
class RunInfo:
    name: str 
    latency_ms: int 
    attr_names: list[str]
    attr_values: list[str]

def run_cpp_program(binary_path: str, args: list[str]) -> RunInfo | ProcessError:
    process = subprocess.run(
        [binary_path] + args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    
    if process.returncode != 0:
        return ProcessError(
            type=ProcessErrorsType.NONE_ZERO_RETURN_CODE, 
            payload=process.stderr
    )

    try:
        data = json.loads(process.stdout)
    except json.JSONDecodeError as e:
        return ProcessError(
            type=ProcessErrorsType.INVALID_JSON, 
            payload=process.stdout
        )

    missing_keys = [
        key for key in ["name", "latency_ms", "attr_names", "attr_values"]
        if key not in data
    ]
    if missing_keys:
        return ProcessError(
            type=ProcessErrorsType.MISSING_FIELDS,
            payload=f"{missing_keys} missing from result"
        )

    return RunInfo(
        name=data["name"],
        latency_ms=data["latency_ms"],
        attr_names=data["attr_names"],
        attr_values=data["attr_values"]
    )

def create_latency_bar_chart(runs: list[RunInfo], title: str="", file_id: str=""):
    categories: list[str] = [run.name for run in runs]
    values: list[int] = [run.latency_ms for run in runs]

    plt.style.use(catppuccin.PALETTE.frappe.identifier)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']

    _, ax = plt.subplots()

    ax.set_axisbelow(True);
    ax.grid(zorder=0)
    ax.bar(categories, values, color=colors[:len(categories)])

    ax.set_xlabel('Version')
    ax.set_ylabel('Latenc (ms)')
    ax.set_title(title)

    plt.savefig(f"profiler/outputs/latency_{file_id}.png", format='png', dpi=300)


if __name__ == "__main__":
    with open("./profiler/inputs/input1.json") as f:
        config = json.load(f)
        
        for chart in config["charts"]:
            title = chart["title"]
            file_id = chart["file_id"]
            runs = [
                run_cpp_program(run["binary_path"], run["args"])
                for run in chart["runs"]
            ]
            create_latency_bar_chart(runs, title=title, file_id=file_id)