"""
=============================================================
  Ensamblador MIPS Simplificado → Código Binario
  Convierte archivos .asm a representación binaria
  Compatible con: VS Code / Terminal y Google Colab
=============================================================
"""

import sys
import os

# ── Detectar entorno ──────────────────────────────────────
def is_colab():
    try:
        import google.colab
        return True
    except ImportError:
        return False

# ── NÚCLEO: Tabla de instrucciones y ensamblador ──────────

# Registros MIPS simplificados
REGISTERS = {f"${i}": i for i in range(32)}
# Aliases comunes
REGISTERS.update({"$zero": 0, "$at": 1, "$v0": 2, "$v1": 3,
                   "$a0": 4, "$a1": 5, "$a2": 6, "$a3": 7,
                   "$t0": 8, "$t1": 9, "$t2": 10, "$t3": 11,
                   "$t4": 12, "$t5": 13, "$t6": 14, "$t7": 15,
                   "$s0": 16, "$s1": 17, "$s2": 18, "$s3": 19,
                   "$s4": 20, "$s5": 21, "$s6": 22, "$s7": 23,
                   "$t8": 24, "$t9": 25, "$k0": 26, "$k1": 27,
                   "$gp": 28, "$sp": 29, "$fp": 30, "$ra": 31})

# Opcodes para instrucciones tipo R (funct), I y J
INSTRUCTION_SET = {
    # Tipo R: opcode=000000, se distingue por funct
    "add":  {"type": "R", "funct": 0b100000, "shamt": 0},
    "sub":  {"type": "R", "funct": 0b100010, "shamt": 0},
    "and":  {"type": "R", "funct": 0b100100, "shamt": 0},
    "or":   {"type": "R", "funct": 0b100101, "shamt": 0},
    "slt":  {"type": "R", "funct": 0b101010, "shamt": 0},
    "nop":  {"type": "R_nop"},  # nop = sll $0,$0,0

    # Tipo I: tienen opcode propio
    "addi": {"type": "I", "opcode": 0b001000},
    "subi": {"type": "I_subi"},          # subi no existe en MIPS real → addi con negativo
    "beq":  {"type": "I", "opcode": 0b000100},
    "bne":  {"type": "I", "opcode": 0b000101},
    "lw":   {"type": "I", "opcode": 0b100011},
    "sw":   {"type": "I", "opcode": 0b101011},

    # Tipo J
    "j":    {"type": "J", "opcode": 0b000010},
    "jal":  {"type": "J", "opcode": 0b000011},
}

def reg(name):
    """Convierte nombre de registro a número."""
    name = name.strip().rstrip(",")
    if name in REGISTERS:
        return REGISTERS[name]
    raise ValueError(f"Registro desconocido: '{name}'")

def to_bin(value, bits):
    """Convierte entero a cadena binaria de longitud fija (complemento a 2)."""
    if value < 0:
        value = value & ((1 << bits) - 1)
    return format(value & ((1 << bits) - 1), f"0{bits}b")

def encode_R(rd, rs, rt, shamt=0, funct=0):
    return "000000" + to_bin(rs,5) + to_bin(rt,5) + to_bin(rd,5) + to_bin(shamt,5) + to_bin(funct,6)

def encode_I(opcode, rs, rt, imm):
    return to_bin(opcode,6) + to_bin(rs,5) + to_bin(rt,5) + to_bin(imm,16)

def encode_J(opcode, addr):
    return to_bin(opcode,6) + to_bin(addr,26)

def first_pass(lines):
    """Recopila etiquetas y sus direcciones de instrucción."""
    labels = {}
    addr = 0
    for line in lines:
        line = line.split("#")[0].strip()
        if not line:
            continue
        if ":" in line:
            label = line.split(":")[0].strip()
            labels[label] = addr
            rest = line.split(":", 1)[1].strip()
            if rest:
                addr += 1
        else:
            addr += 1
    return labels

def assemble_line(line, labels, current_addr):
    """Ensambla una línea y devuelve su representación binaria de 32 bits."""
    line = line.split("#")[0].strip()
    if not line or line.endswith(":"):
        return None

    # Quitar etiqueta si la hay
    if ":" in line:
        line = line.split(":", 1)[1].strip()
    if not line:
        return None

    parts = line.replace(",", " ").split()
    if not parts:
        return None

    instr = parts[0].lower()

    if instr == "nop":
        return encode_R(0, 0, 0, 0, 0)  # sll $0,$0,0

    if instr not in INSTRUCTION_SET:
        raise ValueError(f"Instrucción no reconocida: '{instr}'")

    info = INSTRUCTION_SET[instr]
    t = info["type"]

    # ── Tipo R: add $rd, $rs, $rt ──────────────────────────
    if t == "R":
        rd, rs, rt = reg(parts[1]), reg(parts[2]), reg(parts[3])
        return encode_R(rd, rs, rt, info["shamt"], info["funct"])

    # ── Tipo I estándar: addi $rt, $rs, imm ────────────────
    if t == "I":
        if instr == "beq" or instr == "bne":
            # beq $rs, $rt, label
            rs_v, rt_v = reg(parts[1]), reg(parts[2])
            target = parts[3].strip()
            if target in labels:
                offset = labels[target] - (current_addr + 1)
            else:
                offset = int(target)
            return encode_I(info["opcode"], rs_v, rt_v, offset)
        else:
            rt_v, rs_v = reg(parts[1]), reg(parts[2])
            imm = int(parts[3])
            return encode_I(info["opcode"], rs_v, rt_v, imm)

    # ── subi $rt, $rs, imm → addi con imm negativo ─────────
    if t == "I_subi":
        rt_v, rs_v = reg(parts[1]), reg(parts[2])
        imm = -int(parts[3])
        return encode_I(INSTRUCTION_SET["addi"]["opcode"], rs_v, rt_v, imm)

    # ── Tipo J: j label ─────────────────────────────────────
    if t == "J":
        target = parts[1].strip()
        if target in labels:
            addr = labels[target]
        else:
            addr = int(target)
        return encode_J(info["opcode"], addr)

    raise ValueError(f"Tipo de instrucción desconocido: {t}")

def assemble(source_text):
    """Ensambla texto fuente .asm y retorna lista de (línea_original, binario)."""
    lines = source_text.splitlines()
    labels = first_pass(lines)
    results = []
    current_addr = 0

    for raw_line in lines:
        stripped = raw_line.split("#")[0].strip()
        # Calcular si esta línea genera instrucción
        has_instr = False
        check = stripped
        if ":" in check:
            check = check.split(":", 1)[1].strip()
        if check:
            has_instr = True

        try:
            binary = assemble_line(raw_line, labels, current_addr)
        except Exception as e:
            binary = f"ERROR: {e}"

        if binary is not None:
            results.append((raw_line.rstrip(), binary))
            current_addr += 1
        else:
            results.append((raw_line.rstrip(), ""))   # línea vacía / solo etiqueta / comentario

    return results, labels

def format_output(results, labels):
    """Genera el texto de salida formateado."""
    lines = []
    lines.append("=" * 72)
    lines.append("  ENSAMBLADOR MIPS → CÓDIGO BINARIO")
    lines.append("=" * 72)

    if labels:
        lines.append("\n[ETIQUETAS ENCONTRADAS]")
        for lbl, addr in labels.items():
            lines.append(f"  {lbl:<20} → Dirección {addr} (0x{addr:04X})")

    lines.append("\n[INSTRUCCIONES ENSAMBLADAS]")
    lines.append(f"{'#':<4} {'ASM':<38} {'BINARIO (32 bits)'}")
    lines.append("-" * 72)

    addr = 0
    for asm, binary in results:
        if binary and not binary.startswith("ERROR"):
            # Agrupar en nibbles de 4 bits para legibilidad
            grouped = " ".join(binary[i:i+4] for i in range(0, 32, 4))
            lines.append(f"{addr:<4} {asm:<38} {grouped}")
            addr += 1
        elif binary.startswith("ERROR"):
            lines.append(f"{'??':<4} {asm:<38} ⚠ {binary}")
        else:
            # Línea sin instrucción (comentario / etiqueta)
            lines.append(f"{'  ':<4} {asm}")

    lines.append("=" * 72)
    lines.append(f"Total instrucciones ensambladas: {addr}")
    return "\n".join(lines)

# ══════════════════════════════════════════════════════════
#  INTERFAZ GRÁFICA (tkinter) — para VS Code / Terminal
# ══════════════════════════════════════════════════════════

def run_gui():
    import tkinter as tk
    from tkinter import filedialog, messagebox, scrolledtext, font as tkFont

    # Paleta de colores estilo terminal/retro
    BG      = "#0d1117"
    BG2     = "#161b22"
    ACCENT  = "#00ff9d"
    ACCENT2 = "#00bfff"
    TEXT    = "#c9d1d9"
    MUTED   = "#8b949e"
    BTN_BG  = "#21262d"
    BTN_HOV = "#30363d"
    ERROR   = "#ff6b6b"

    root = tk.Tk()
    root.title("ASM → Binario  |  MIPS Assembler")
    root.geometry("1050x720")
    root.configure(bg=BG)
    root.resizable(True, True)

    # ── Fuentes ──────────────────────────────────────────
    try:
        fMono  = tkFont.Font(family="Consolas",   size=11)
        fTitle = tkFont.Font(family="Consolas",   size=15, weight="bold")
        fBtn   = tkFont.Font(family="Consolas",   size=11, weight="bold")
        fSmall = tkFont.Font(family="Consolas",   size=9)
    except:
        fMono  = tkFont.Font(family="Courier",    size=11)
        fTitle = tkFont.Font(family="Courier",    size=15, weight="bold")
        fBtn   = tkFont.Font(family="Courier",    size=11, weight="bold")
        fSmall = tkFont.Font(family="Courier",    size=9)

    # ── Estado ───────────────────────────────────────────
    current_file = tk.StringVar(value="Ningún archivo cargado")
    status_msg   = tk.StringVar(value="Listo.")

    # ── Header ───────────────────────────────────────────
    header = tk.Frame(root, bg=BG, pady=14)
    header.pack(fill="x", padx=20)

    tk.Label(header, text="⚙  MIPS ASSEMBLER", font=fTitle,
             bg=BG, fg=ACCENT).pack(side="left")
    tk.Label(header, text="ASM → Código Binario", font=fSmall,
             bg=BG, fg=MUTED).pack(side="left", padx=16, pady=4)

    # ── Separador ────────────────────────────────────────
    tk.Frame(root, bg=ACCENT, height=2).pack(fill="x", padx=20)

    # ── Barra de archivo ─────────────────────────────────
    file_bar = tk.Frame(root, bg=BG2, pady=10, padx=14)
    file_bar.pack(fill="x", padx=20, pady=(8,0))

    tk.Label(file_bar, text="📂  Archivo:", font=fBtn,
             bg=BG2, fg=ACCENT2).pack(side="left")
    tk.Label(file_bar, textvariable=current_file, font=fSmall,
             bg=BG2, fg=TEXT, wraplength=600).pack(side="left", padx=10)

    # ── Panel de botones ─────────────────────────────────
    btn_bar = tk.Frame(root, bg=BG, pady=8)
    btn_bar.pack(fill="x", padx=20)

    def make_btn(parent, text, cmd, color=ACCENT, hover=BTN_HOV):
        b = tk.Button(parent, text=text, command=cmd,
                      bg=BTN_BG, fg=color, font=fBtn,
                      relief="flat", bd=0, padx=18, pady=8,
                      activebackground=hover, activeforeground=color,
                      cursor="hand2")
        b.pack(side="left", padx=6)
        b.bind("<Enter>", lambda e: b.configure(bg=hover))
        b.bind("<Leave>", lambda e: b.configure(bg=BTN_BG))
        return b

    # ── Paneles de texto ─────────────────────────────────
    panels = tk.Frame(root, bg=BG)
    panels.pack(fill="both", expand=True, padx=20, pady=8)

    # — Panel izquierdo: código ASM —
    left = tk.Frame(panels, bg=BG2, bd=0)
    left.pack(side="left", fill="both", expand=True, padx=(0,6))

    tk.Label(left, text=" 〈 CÓDIGO FUENTE .ASM 〉", font=fBtn,
             bg=BG2, fg=ACCENT2, anchor="w").pack(fill="x", padx=8, pady=(8,2))

    asm_text = scrolledtext.ScrolledText(left, font=fMono, bg="#0d1117",
                                          fg=TEXT, insertbackground=ACCENT,
                                          selectbackground=ACCENT2,
                                          relief="flat", bd=0,
                                          wrap="none", height=24)
    asm_text.pack(fill="both", expand=True, padx=4, pady=(0,4))

    # — Panel derecho: resultado binario —
    right = tk.Frame(panels, bg=BG2, bd=0)
    right.pack(side="right", fill="both", expand=True, padx=(6,0))

    tk.Label(right, text=" 〈 SALIDA BINARIA 〉", font=fBtn,
             bg=BG2, fg=ACCENT, anchor="w").pack(fill="x", padx=8, pady=(8,2))

    out_text = scrolledtext.ScrolledText(right, font=fMono, bg="#0d1117",
                                          fg=ACCENT, insertbackground=ACCENT,
                                          selectbackground=ACCENT2,
                                          relief="flat", bd=0,
                                          wrap="none", height=24)
    out_text.pack(fill="both", expand=True, padx=4, pady=(0,4))

    # ── Status bar ───────────────────────────────────────
    tk.Frame(root, bg=ACCENT, height=1).pack(fill="x", padx=20)
    status_bar = tk.Label(root, textvariable=status_msg, font=fSmall,
                          bg=BG2, fg=MUTED, anchor="w", pady=5, padx=14)
    status_bar.pack(fill="x", padx=20)

    # ── Funciones de acción ───────────────────────────────

    def cargar_archivo():
        path = filedialog.askopenfilename(
            title="Selecciona archivo .asm",
            filetypes=[("Archivos ASM", "*.asm *.txt"), ("Todos", "*.*")]
        )
        if not path:
            return
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            content = f.read()
        asm_text.delete("1.0", "end")
        asm_text.insert("1.0", content)
        current_file.set(os.path.basename(path))
        out_text.delete("1.0", "end")
        status_msg.set(f"✔  Archivo cargado: {path}")

    def ensamblar():
        source = asm_text.get("1.0", "end").strip()
        if not source:
            messagebox.showwarning("Sin código", "Carga o escribe código ASM primero.")
            return
        try:
            results, labels = assemble(source)
            output = format_output(results, labels)
            out_text.delete("1.0", "end")
            out_text.insert("1.0", output)
            status_msg.set("✔  Ensamblado exitoso.")
            status_bar.configure(fg=ACCENT)
        except Exception as e:
            out_text.delete("1.0", "end")
            out_text.insert("1.0", f"ERROR AL ENSAMBLAR:\n{e}")
            status_msg.set(f"✘  Error: {e}")
            status_bar.configure(fg=ERROR)

    def guardar_salida():
        content = out_text.get("1.0", "end").strip()
        if not content:
            messagebox.showwarning("Sin salida", "Ensambla primero antes de guardar.")
            return
        path = filedialog.asksaveasfilename(
            title="Guardar binario como…",
            defaultextension=".txt",
            filetypes=[("Texto", "*.txt"), ("Todos", "*.*")]
        )
        if not path:
            return
        with open(path, "w", encoding="utf-8") as f:
            f.write(content)
        status_msg.set(f"✔  Guardado en: {path}")

    def limpiar():
        asm_text.delete("1.0", "end")
        out_text.delete("1.0", "end")
        current_file.set("Ningún archivo cargado")
        status_msg.set("Listo.")
        status_bar.configure(fg=MUTED)

    def cargar_ejemplo():
        ejemplo = """\
# Archivo: multiplicacion.asm
# Algoritmo: Multiplicación por sumas sucesivas (P = A * B)
# R1 = Multiplicando (A)
# R2 = Multiplicador (B)
# R3 = Resultado
# R0 = Constante 0

add $3, $0, $0      # Inicializa el resultado en 0

Loop:
beq $2, $0, Exit    # Si el multiplicador (R2) es 0, termina el ciclo
add $3, $3, $1      # Resultado = Resultado + Multiplicando
subi $2, $2, 1      # Decrementa el multiplicador en 1
j Loop              # Salta de regreso al inicio del ciclo

Exit:
nop                 # Fin del programa"""
        asm_text.delete("1.0", "end")
        asm_text.insert("1.0", ejemplo)
        current_file.set("multiplicacion.asm  [ejemplo integrado]")
        out_text.delete("1.0", "end")
        status_msg.set("✔  Ejemplo de multiplicación.asm cargado.")

    # ── Botones ───────────────────────────────────────────
    make_btn(btn_bar, "📂  Cargar .asm",        cargar_archivo, ACCENT2)
    make_btn(btn_bar, "▶  Ejemplo integrado",    cargar_ejemplo, MUTED)
    make_btn(btn_bar, "⚙  Ensamblar → Binario", ensamblar,      ACCENT)
    make_btn(btn_bar, "💾  Guardar .txt",        guardar_salida, "#ffa657")
    make_btn(btn_bar, "🗑  Limpiar",             limpiar,        ERROR)

    root.mainloop()

# ══════════════════════════════════════════════════════════
#  INTERFAZ COLAB — widgets ipython
# ══════════════════════════════════════════════════════════

def run_colab():
    import ipywidgets as widgets
    from IPython.display import display, HTML

    style = """
    <style>
      .asm-title { font-family: monospace; font-size: 20px; color: #00ff9d;
                   background:#0d1117; padding:12px 20px; border-radius:8px;
                   border-left: 4px solid #00ff9d; margin-bottom:10px; }
      .asm-subtitle { font-family: monospace; color:#8b949e; font-size:13px; }
    </style>
    <div class="asm-title">⚙  MIPS ASSEMBLER — ASM → Binario</div>
    <div class="asm-subtitle">Carga tu archivo .asm o usa el ejemplo integrado</div>
    """
    display(HTML(style))

    # Upload widget
    upload = widgets.FileUpload(accept=".asm,.txt", multiple=False,
                                 description="📂 Cargar .asm",
                                 layout=widgets.Layout(width="200px"))

    btn_ejemplo  = widgets.Button(description="▶ Ejemplo integrado",
                                   button_style="info",
                                   layout=widgets.Layout(width="180px"))
    btn_ensamblar = widgets.Button(description="⚙ Ensamblar",
                                    button_style="success",
                                    layout=widgets.Layout(width="150px"))
    btn_guardar  = widgets.Button(description="💾 Guardar .txt",
                                   button_style="warning",
                                   layout=widgets.Layout(width="150px"))

    asm_area = widgets.Textarea(
        placeholder="Pega o carga tu código .asm aquí…",
        layout=widgets.Layout(width="99%", height="260px"),
    )
    out_area = widgets.Textarea(
        placeholder="La salida binaria aparecerá aquí…",
        layout=widgets.Layout(width="99%", height="260px"),
    )
    status = widgets.HTML(value="<span style='color:#8b949e;font-family:monospace'>Listo.</span>")

    EJEMPLO_ASM = """\
# Archivo: multiplicacion.asm
# Algoritmo: Multiplicación por sumas sucesivas (P = A * B)
# R1 = Multiplicando (A)
# R2 = Multiplicador (B)
# R3 = Resultado
# R0 = Constante 0

add $3, $0, $0      # Inicializa el resultado en 0

Loop:
beq $2, $0, Exit    # Si el multiplicador (R2) es 0, termina el ciclo
add $3, $3, $1      # Resultado = Resultado + Multiplicando
subi $2, $2, 1      # Decrementa el multiplicador en 1
j Loop              # Salta de regreso al inicio del ciclo

Exit:
nop                 # Fin del programa"""

    def on_upload(change):
        if upload.value:
            fname = list(upload.value.keys())[0]
            content = upload.value[fname]["content"]
            asm_area.value = content.decode("utf-8", errors="replace")
            status.value = f"<span style='color:#00bfff;font-family:monospace'>✔ Archivo cargado: {fname}</span>"

    def on_ejemplo(b):
        asm_area.value = EJEMPLO_ASM
        out_area.value = ""
        status.value = "<span style='color:#8b949e;font-family:monospace'>✔ Ejemplo multiplicacion.asm cargado.</span>"

    def on_ensamblar(b):
        source = asm_area.value.strip()
        if not source:
            status.value = "<span style='color:#ff6b6b;font-family:monospace'>✘ Escribe o carga código ASM primero.</span>"
            return
        try:
            results, labels = assemble(source)
            output = format_output(results, labels)
            out_area.value = output
            status.value = "<span style='color:#00ff9d;font-family:monospace'>✔ Ensamblado exitoso.</span>"
        except Exception as e:
            out_area.value = f"ERROR:\n{e}"
            status.value = f"<span style='color:#ff6b6b;font-family:monospace'>✘ Error: {e}</span>"

    def on_guardar(b):
        content = out_area.value.strip()
        if not content:
            status.value = "<span style='color:#ff6b6b;font-family:monospace'>✘ Ensambla primero.</span>"
            return
        path = "salida_binario.txt"
        with open(path, "w", encoding="utf-8") as f:
            f.write(content)
        status.value = f"<span style='color:#ffa657;font-family:monospace'>✔ Guardado como '{path}' en el directorio de Colab.</span>"

    upload.observe(on_upload, names="value")
    btn_ejemplo.on_click(on_ejemplo)
    btn_ensamblar.on_click(on_ensamblar)
    btn_guardar.on_click(on_guardar)

    display(widgets.HBox([upload, btn_ejemplo, btn_ensamblar, btn_guardar]))
    display(HTML("<b style='font-family:monospace;color:#00bfff'>Código Fuente ASM:</b>"))
    display(asm_area)
    display(HTML("<b style='font-family:monospace;color:#00ff9d'>Salida Binaria:</b>"))
    display(out_area)
    display(status)


# ══════════════════════════════════════════════════════════
#  MODO CONSOLA — fallback sin GUI
# ══════════════════════════════════════════════════════════

def run_console():
    print("=" * 60)
    print("  MIPS ASSEMBLER — ASM → Binario  (modo consola)")
    print("=" * 60)
    path = input("Ruta del archivo .asm: ").strip().strip('"')
    if not os.path.isfile(path):
        print(f"Archivo no encontrado: {path}")
        return
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        source = f.read()
    results, labels = assemble(source)
    output = format_output(results, labels)
    print(output)
    save = input("\n¿Guardar resultado en archivo .txt? (s/n): ").strip().lower()
    if save == "s":
        out_path = path.rsplit(".", 1)[0] + "_binario.txt"
        with open(out_path, "w", encoding="utf-8") as f:
            f.write(output)
        print(f"Guardado en: {out_path}")


# ══════════════════════════════════════════════════════════
#  PUNTO DE ENTRADA
# ══════════════════════════════════════════════════════════

if __name__ == "__main__":
    if is_colab():
        run_colab()
    else:
        try:
            import tkinter
            run_gui()
        except ImportError:
            print("tkinter no disponible, usando modo consola.")
            run_console()
