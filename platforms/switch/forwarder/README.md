# Panduan Pembuatan Forwarder NSP (Sapphire / NRO2NSP) untuk ARMSX2

Forwarder NSP memungkinkan Anda membuka ARMSX2 langsung dari Home Menu Nintendo Switch tanpa perlu masuk ke Homebrew Menu (Album / Hold R), serta otomatis memberikan akses memori penuh (3.2 GB RAM) dan izin eksekusi JIT Atmosphere.

---

## 1. Pembuatan Forwarder Utama ARMSX2

### Menggunakan Sapphire (Web / Desktop Tool)
1. Buka aplikasi **Sapphire Forwarder Generator** (atau alat pembuat NRO-to-NSP seperti NSCBuilder / NRO2NSP).
2. Isi parameter berikut:
   - **Title Name:** `ARMSX2`
   - **Author:** `ARMSX2 Team`
   - **Title ID:** `0100A64000582000` (harus unik dan berawalan `0100`)
   - **NRO Path:** `sdmc:/switch/armsx2/armsx2.nro`
   - **Icon:** Gunakan file `platforms/switch/assets/icon.png` (resolusi 256x256).
   - **Application Type:** Pilih `Application` (bukan Applet) untuk memastikan Switch mengalokasikan RAM penuh (~3.2 GB).
   - **Enable JIT Flag:** Centang opsi JIT / ControlCodeMemory permission.
3. Klik **Generate NSP**.
4. Salin file `.nsp` yang dihasilkan ke kartu SD Switch, lalu pasang menggunakan installer favorit Anda (**DBI**, **Tinfoil**, atau **Goldleaf**).

---

## 2. Pembuatan Forwarder Langsung ke Game Tertentu (Direct-Boot Game Forwarder)

Jika Anda ingin membuat shortcut di Home Menu yang langsung membuka game PS2 tertentu (misalnya *Persona 4* atau *God of War*):

1. Di Sapphire, atur **Target NRO Path**:
   ```text
   sdmc:/switch/armsx2/armsx2.nro
   ```
2. Isi kolom **Arguments / argv**:
   ```text
   "sdmc:/roms/ps2/Persona 4.iso"
   ```
3. Gunakan Title ID baru yang unik (misalnya `0100A64000582001`).
4. Gunakan cover art game sebagai ikon (256x256).
5. Generate dan install file NSP tersebut. Saat Anda menekan ikon game di Home Menu, ARMSX2 akan otomatis langsung meluncurkan ISO tersebut!
