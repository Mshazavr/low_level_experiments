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

def run_cpp_program(binary_path: str, args: list[str]) -> list[RunInfo] | ProcessError:
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
        output_json = json.loads(process.stdout)
    except json.JSONDecodeError as e:
        return ProcessError(
            type=ProcessErrorsType.INVALID_JSON, 
            payload=process.stdout
        )

    if not isinstance(output_json, list):
        output_json = [output_json]
        
    run_infos = []
    for run_info in output_json:
        missing_keys = [
            key for key in ["name", "latency_ms", "attr_names", "attr_values"]
            if key not in run_info
        ]
        if missing_keys:
            return ProcessError(
                type=ProcessErrorsType.MISSING_FIELDS,
                payload=f"{missing_keys} missing from result"
            )

        run_infos.append(RunInfo(**run_info))

    return run_infos

def create_latency_bar_chart(runs: list[RunInfo], title: str="", file_id: str=""):
    categories: list[str] = [run.name for run in runs]
    values: list[int] = [run.latency_ms for run in runs]

    plt.style.use(catppuccin.PALETTE.frappe.identifier)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']

    _, ax = plt.subplots()

    ax.set_axisbelow(True)
    ax.grid(zorder=0)
    ax.bar(categories, values, color=colors[:len(categories)])

    ax.set_xlabel('Version')
    ax.set_ylabel('Latenc (ms)')
    ax.set_title(title)
    
    plt.setp(ax.get_xticklabels(), rotation = 45)
    plt.savefig(f"profiler/outputs/latency_{file_id}.png", format='png', bbox_inches = 'tight', dpi=300)


def create_latency_plot(runs: list[list[RunInfo]], title: str="", file_id: str=""):
    # TODO don't use hardcoded indices to access the right attributes from RunInfo
    plt.style.use(catppuccin.PALETTE.frappe.identifier)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']

    _, ax = plt.subplots()
    ax.set_axisbelow(True)
    ax.grid(zorder=0)
    for i, run in enumerate(runs):
        line_x = [float(info.attr_values[1]) / 1000.0 for info in run]
        line_y = [float(info.latency_ms) for info in run]
        ax.plot(line_x, line_y, marker='o', color=colors[i % len(colors)])
    
    ax.axvline(float(runs[0][0].attr_values[2]) / 1000.0, color='lightgrey', linestyle='-', label="L1d (kb)", alpha=0.5)
    ax.axvline(float(runs[0][0].attr_values[3]) / 1000.0, color='grey', linestyle='-', label="L2 (kb)", alpha=0.5)
    ax.axvline(float(runs[0][0].attr_values[4]) / 1000.0, color='darkgrey',  linestyle='-', label="L3 (kb)", alpha=0.5)

    ax.set_xscale('log', base=2);

    ax.set_xlabel("Data size (kb)")
    ax.set_ylabel('Latenc (ms)')
    ax.legend(loc="upper right")

    plt.savefig(f"profiler/outputs/latency_{file_id}.png", format='png', bbox_inches = 'tight', dpi=300)


if __name__ == "__main__":
    with open("./profiler/inputs/reduce_input.json") as f:
        config = json.load(f)
        
        for chart in config["charts"]:
            title = chart["title"]
            file_id = chart["file_id"]
            runs = [
                run_cpp_program(run["binary_path"], run["args"])
                for run in chart["runs"]
            ]
            if chart["chart_type"] == "bar":
                create_latency_bar_chart([r[0] for r in runs], title=title, file_id=file_id)
            elif chart["chart_type"] == "plot":
                create_latency_plot(runs, title=title, file_id=file_id)