# --- INICIALIZACIÓN ---
addi $t0, $zero, 25    # $t0 = 25 (Número a evaluar)
addi $t1, $zero, 1     # $t1 = 1  (Primer número impar)
addi $t2, $zero, 0     # $t2 = 0  (Contador, guardará el resultado de la raíz)
addi $t4, $zero, 1     # $t4 = 1  (Constante de comparación)

# --- BUCLE PRINCIPAL (LOOP) ---
slt  $t3, $t0, $t1     # $t3 = 1 si ($t0 < $t1), de lo contrario 0 
beq  $t3, $t4, 4       # Si $t3 == 1 (Fin del cálculo), salta al Fin (4 inst. adelante)

# --- CUERPO DEL BUCLE ---
sub  $t0, $t0, $t1     # Número = Número - Impar_Actual
addi $t2, $t2, 1       # Raíz_Contador = Raíz_Contador + 1
addi $t1, $t1, 2       # Impar_Actual = Impar_Actual + 2 (Siguiente impar)

# --- RETORNO DE BUCLE ---
beq  $zero, $zero, -6  # Salto incondicional forzado: Regresa al inicio del bucle (SLT)

# --- FIN DEL PROGRAMA ---
nop                    # El resultado de la raíz cuadrada de 25 reposa en el registro $t2 (Valor esperado: 5)
