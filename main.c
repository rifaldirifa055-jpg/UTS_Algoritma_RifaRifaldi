#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAKS_PRODUK  50   /* kapasitas maksimal array */
#define PANJANG_NAMA 30   /* panjang maksimal nama produk (termasuk '\0') */
#define BATAS_MENIPIS 5   /* stok < 5 dianggap menipis */

/* ===================== DATA GLOBAL (ARRAY) ===================== */
char nama[MAKS_PRODUK][PANJANG_NAMA];
int  harga[MAKS_PRODUK];
int  stok[MAKS_PRODUK];
int  terjual[MAKS_PRODUK];
long pendapatan[MAKS_PRODUK];
int  jumlahProduk = 0;

/* ===================== FUNGSI BANTU INPUT ===================== */

/* Membaca satu baris teks dari pengguna. Mengembalikan 0 jika EOF. */
int bacaBaris(const char *prompt, char *buf, int ukuran)
{
    printf("%s", prompt);
    if (fgets(buf, ukuran, stdin) == NULL) {
        return 0;
    }
    /* jika baris lebih panjang dari buffer, buang sisa input agar tidak terbawa */
    if (strchr(buf, '\n') == NULL && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

/* Membaca bilangan bulat dengan validasi (menolak huruf/kosong).
 * Mengembalikan 1 jika berhasil, 0 jika input bukan bilangan bulat. */
int bacaInt(const char *prompt, int *hasil)
{
    char buf[64];
    char *akhir;
    long nilai;

    if (!bacaBaris(prompt, buf, sizeof(buf))) {
        printf("\nInput berakhir. Program ditutup.\n");
        exit(0);
    }
    if (buf[0] == '\0') {
        printf("[!] Input tidak boleh kosong.\n");
        return 0;
    }
    nilai = strtol(buf, &akhir, 10);
    while (*akhir == ' ') akhir++;            /* abaikan spasi di belakang */
    if (*akhir != '\0' || nilai > 2000000000L || nilai < -2000000000L) {
        printf("[!] Input harus berupa bilangan bulat yang valid.\n");
        return 0;
    }
    *hasil = (int)nilai;
    return 1;
}

/* Membaca string non-kosong (nama produk) */
int bacaNama(const char *prompt, char *tujuan)
{
    char buf[PANJANG_NAMA + 40];
    int i, panjang;

    if (!bacaBaris(prompt, buf, sizeof(buf))) {
        printf("\nInput berakhir. Program ditutup.\n");
        exit(0);
    }
    /* hapus spasi di awal & akhir */
    panjang = (int)strlen(buf);
    while (panjang > 0 && buf[panjang - 1] == ' ') buf[--panjang] = '\0';
    i = 0;
    while (buf[i] == ' ') i++;
    if (buf[i] == '\0') {
        printf("[!] Nama tidak boleh kosong.\n");
        return 0;
    }
    if (strlen(buf + i) >= PANJANG_NAMA) {
        printf("[!] Nama terlalu panjang (maksimal %d karakter).\n", PANJANG_NAMA - 1);
        return 0;
    }
    strcpy(tujuan, buf + i);
    return 1;
}

/* ===================== FUNGSI BANTU TAMPILAN ===================== */

/* Mengubah angka menjadi format rupiah, mis. 12500 -> "Rp 12.500" */
void formatRupiah(long angka, char *hasil)
{
    char digit[32], balik[48];
    int n, i, k = 0;

    sprintf(digit, "%ld", angka < 0 ? -angka : angka);
    n = (int)strlen(digit);
    for (i = 0; i < n; i++) {
        if (i > 0 && (n - i) % 3 == 0) balik[k++] = '.';
        balik[k++] = digit[i];
    }
    balik[k] = '\0';
    sprintf(hasil, "%sRp %s", angka < 0 ? "-" : "", balik);
}

void garis(int panjang)
{
    int i;
    for (i = 0; i < panjang; i++) putchar('-');
    putchar('\n');
}

/* Mengubah string menjadi huruf kecil (untuk pencarian tanpa peduli kapital) */
void keHurufKecil(const char *asal, char *tujuan)
{
    int i;
    for (i = 0; asal[i] != '\0'; i++) {
        tujuan[i] = (char)tolower((unsigned char)asal[i]);
    }
    tujuan[i] = '\0';
}

/* Memeriksa nomor produk (1..jumlahProduk). Mengembalikan indeks atau -1. */
int nomorKeIndeks(int nomor)
{
    if (nomor < 1 || nomor > jumlahProduk) {
        printf("[!] Nomor produk %d tidak ditemukan (valid: 1-%d).\n", nomor, jumlahProduk);
        return -1;
    }
    return nomor - 1;
}

/* ===================== DATA AWAL ===================== */

void tambahDataAwal(const char *n, int h, int s)
{
    strcpy(nama[jumlahProduk], n);
    harga[jumlahProduk]      = h;
    stok[jumlahProduk]       = s;
    terjual[jumlahProduk]    = 0;
    pendapatan[jumlahProduk] = 0;
    jumlahProduk++;
}

void inisialisasiData(void)
{
    tambahDataAwal("Nasi Goreng",  12000, 20);
    tambahDataAwal("Mie Ayam",     10000, 15);
    tambahDataAwal("Es Teh Manis",  3000, 30);
    tambahDataAwal("Air Mineral",   4000,  4);   /* sengaja < 5 untuk uji stok menipis */
    tambahDataAwal("Gorengan",      2000, 40);
    tambahDataAwal("Kopi Hitam",    5000, 12);
}

/* ===================== FITUR 1: LIHAT DAFTAR ===================== */

void tampilBarisProduk(int i)
{
    char rp[32];
    formatRupiah(harga[i], rp);
    printf("%-4d %-*s %14s %6d\n", i + 1, PANJANG_NAMA, nama[i], rp, stok[i]);
}

void tampilDaftarProduk(void)
{
    int i;
    printf("\n=== DAFTAR PRODUK (%d/%d) ===\n", jumlahProduk, MAKS_PRODUK);
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    garis(56);
    printf("%-4s %-*s %14s %6s\n", "No", PANJANG_NAMA, "Nama Produk", "Harga", "Stok");
    garis(56);
    for (i = 0; i < jumlahProduk; i++) {
        tampilBarisProduk(i);
    }
    garis(56);
}

/* ===================== FITUR 2: TAMBAH PRODUK ===================== */

int namaSudahAda(const char *cari)
{
    char a[PANJANG_NAMA], b[PANJANG_NAMA];
    int i;
    keHurufKecil(cari, a);
    for (i = 0; i < jumlahProduk; i++) {
        keHurufKecil(nama[i], b);
        if (strcmp(a, b) == 0) return 1;
    }
    return 0;
}

void tambahProduk(void)
{
    char namaBaru[PANJANG_NAMA];
    int hargaBaru, stokBaru;

    printf("\n=== TAMBAH PRODUK ===\n");
    if (jumlahProduk >= MAKS_PRODUK) {
        printf("[!] Kapasitas penuh! Maksimal %d produk.\n", MAKS_PRODUK);
        return;
    }
    if (!bacaNama("Nama produk : ", namaBaru)) return;
    if (namaSudahAda(namaBaru)) {
        printf("[!] Produk \"%s\" sudah ada dalam daftar.\n", namaBaru);
        return;
    }
    if (!bacaInt("Harga (Rp) : ", &hargaBaru)) return;
    if (hargaBaru <= 0) {
        printf("[!] Harga harus lebih dari 0 (tidak boleh negatif).\n");
        return;
    }
    if (!bacaInt("Stok awal   : ", &stokBaru)) return;
    if (stokBaru < 0) {
        printf("[!] Stok tidak boleh negatif.\n");
        return;
    }

    strcpy(nama[jumlahProduk], namaBaru);
    harga[jumlahProduk]      = hargaBaru;
    stok[jumlahProduk]       = stokBaru;
    terjual[jumlahProduk]    = 0;
    pendapatan[jumlahProduk] = 0;
    jumlahProduk++;
    printf("[OK] Produk \"%s\" berhasil ditambahkan sebagai nomor %d.\n", namaBaru, jumlahProduk);
}

/* ===================== FITUR 3: UBAH PRODUK ===================== */

void ubahProduk(void)
{
    int nomor, idx, hargaBaru, stokBaru;

    printf("\n=== UBAH PRODUK ===\n");
    tampilDaftarProduk();
    if (jumlahProduk == 0) return;
    if (!bacaInt("Nomor produk yang diubah : ", &nomor)) return;
    idx = nomorKeIndeks(nomor);
    if (idx < 0) return;

    printf("Produk terpilih: %s (harga lama %d, stok lama %d)\n", nama[idx], harga[idx], stok[idx]);
    if (!bacaInt("Harga baru (Rp) : ", &hargaBaru)) return;
    if (hargaBaru <= 0) {
        printf("[!] Harga harus lebih dari 0 (tidak boleh negatif). Perubahan dibatalkan.\n");
        return;
    }
    if (!bacaInt("Stok baru       : ", &stokBaru)) return;
    if (stokBaru < 0) {
        printf("[!] Stok tidak boleh negatif. Perubahan dibatalkan.\n");
        return;
    }
    harga[idx] = hargaBaru;
    stok[idx]  = stokBaru;
    printf("[OK] Data \"%s\" berhasil diubah.\n", nama[idx]);
}

/* ===================== FITUR 4: TRANSAKSI PENJUALAN ===================== */

void transaksiPenjualan(void)
{
    int belanja[MAKS_PRODUK];   /* jumlah beli per produk pada transaksi ini */
    int i, nomor, idx, qty, adaItem = 0;
    int totalUnit = 0;
    long subtotal, total = 0;
    char konfirmasi[16], rp1[32], rp2[32];

    printf("\n=== TRANSAKSI PENJUALAN ===\n");
    if (jumlahProduk == 0) {
        printf("[!] Belum ada produk untuk dijual.\n");
        return;
    }
    for (i = 0; i < MAKS_PRODUK; i++) belanja[i] = 0;

    tampilDaftarProduk();
    printf("Masukkan nomor produk dan jumlah. Ketik 0 pada nomor produk untuk selesai.\n");

    while (1) {
        if (!bacaInt("\nNomor produk (0 = selesai) : ", &nomor)) continue;
        if (nomor == 0) break;
        idx = nomorKeIndeks(nomor);
        if (idx < 0) continue;

        printf("Produk: %s | stok tersedia: %d\n", nama[idx], stok[idx] - belanja[idx]);
        if (!bacaInt("Jumlah beli : ", &qty)) continue;
        if (qty <= 0) {
            printf("[!] Jumlah pembelian harus lebih dari 0.\n");
            continue;
        }
        if (qty + belanja[idx] > stok[idx]) {
            printf("[!] Stok tidak cukup! Sisa stok %s: %d, diminta: %d.\n",
                   nama[idx], stok[idx] - belanja[idx], qty);
            continue;
        }
        belanja[idx] += qty;
        adaItem = 1;
        printf("[OK] %d x %s ditambahkan ke keranjang.\n", qty, nama[idx]);
    }

    if (!adaItem) {
        printf("Keranjang kosong. Transaksi dibatalkan.\n");
        return;
    }

    /* Cetak rincian & hitung total */
    printf("\n================ RINCIAN TRANSAKSI ================\n");
    printf("%-*s %6s %12s %14s\n", PANJANG_NAMA, "Produk", "Qty", "Harga", "Subtotal");
    garis(66);
    for (i = 0; i < jumlahProduk; i++) {
        if (belanja[i] > 0) {
            subtotal = (long)belanja[i] * harga[i];
            formatRupiah(harga[i], rp1);
            formatRupiah(subtotal, rp2);
            printf("%-*s %6d %12s %14s\n", PANJANG_NAMA, nama[i], belanja[i], rp1, rp2);
            total += subtotal;
            totalUnit += belanja[i];
        }
    }
    garis(66);
    formatRupiah(total, rp1);
    printf("%-*s %6d %12s %14s\n", PANJANG_NAMA, "TOTAL PEMBAYARAN", totalUnit, "", rp1);

    if (!bacaBaris("\nProses pembayaran? (y/n) : ", konfirmasi, sizeof(konfirmasi))) exit(0);
    if (konfirmasi[0] != 'y' && konfirmasi[0] != 'Y') {
        printf("Transaksi dibatalkan. Stok tidak berubah.\n");
        return;
    }

    /* Transaksi berhasil: kurangi stok, tambah terjual & pendapatan */
    for (i = 0; i < jumlahProduk; i++) {
        if (belanja[i] > 0) {
            stok[i]       -= belanja[i];
            terjual[i]    += belanja[i];
            pendapatan[i] += (long)belanja[i] * harga[i];
        }
    }
    printf("[OK] Transaksi berhasil. Stok dan data penjualan telah diperbarui.\n");
}

/* ===================== FITUR 5: TAMBAH STOK ===================== */

void tambahStok(void)
{
    int nomor, idx, tambahan;

    printf("\n=== TAMBAH STOK ===\n");
    tampilDaftarProduk();
    if (jumlahProduk == 0) return;
    if (!bacaInt("Nomor produk : ", &nomor)) return;
    idx = nomorKeIndeks(nomor);
    if (idx < 0) return;
    if (!bacaInt("Jumlah stok yang ditambahkan : ", &tambahan)) return;
    if (tambahan <= 0) {
        printf("[!] Jumlah tambahan stok harus lebih dari 0.\n");
        return;
    }
    stok[idx] += tambahan;
    printf("[OK] Stok %s bertambah %d. Stok sekarang: %d.\n", nama[idx], tambahan, stok[idx]);
}

/* ===================== FITUR 6: CARI PRODUK ===================== */

void cariProduk(void)
{
    char kata[PANJANG_NAMA], kataKecil[PANJANG_NAMA], namaKecil[PANJANG_NAMA];
    int i, ketemu = 0;

    printf("\n=== CARI PRODUK ===\n");
    if (!bacaNama("Masukkan nama / sebagian nama : ", kata)) return;
    keHurufKecil(kata, kataKecil);

    /* pencarian linear: cocok jika kata ada di dalam nama (tanpa peduli kapital) */
    for (i = 0; i < jumlahProduk; i++) {
        keHurufKecil(nama[i], namaKecil);
        if (strstr(namaKecil, kataKecil) != NULL) {
            if (!ketemu) {
                garis(56);
                printf("%-4s %-*s %14s %6s\n", "No", PANJANG_NAMA, "Nama Produk", "Harga", "Stok");
                garis(56);
            }
            tampilBarisProduk(i);
            ketemu++;
        }
    }
    if (ketemu) {
        garis(56);
        printf("Ditemukan %d produk.\n", ketemu);
    } else {
        printf("Produk dengan kata \"%s\" tidak ditemukan.\n", kata);
    }
}

/* ===================== FITUR 7: URUTKAN PRODUK ===================== */

/* Mengurutkan array indeks urutan[] dengan selection sort.
 * mode 1: harga termurah -> termahal
 * mode 2: jumlah terjual terbanyak -> tersedikit */
void urutkanIndeks(int urutan[], int mode)
{
    int i, j, pilih, temp, tukar;

    for (i = 0; i < jumlahProduk - 1; i++) {
        pilih = i;
        for (j = i + 1; j < jumlahProduk; j++) {
            if (mode == 1) tukar = harga[urutan[j]] < harga[urutan[pilih]];
            else           tukar = terjual[urutan[j]] > terjual[urutan[pilih]];
            if (tukar) pilih = j;
        }
        if (pilih != i) {
            temp = urutan[i];
            urutan[i] = urutan[pilih];
            urutan[pilih] = temp;
        }
    }
}

void urutkanProduk(void)
{
    int urutan[MAKS_PRODUK];
    int i, pilihan;
    char rp[32];

    printf("\n=== URUTKAN PRODUK ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    printf("1. Harga termurah ke termahal\n");
    printf("2. Jumlah terjual terbanyak ke tersedikit\n");
    if (!bacaInt("Pilih jenis pengurutan : ", &pilihan)) return;
    if (pilihan != 1 && pilihan != 2) {
        printf("[!] Pilihan pengurutan tidak tersedia.\n");
        return;
    }

    for (i = 0; i < jumlahProduk; i++) urutan[i] = i;
    urutkanIndeks(urutan, pilihan);

    printf("\nHasil pengurutan berdasarkan %s:\n",
           pilihan == 1 ? "harga (termurah)" : "jumlah terjual (terbanyak)");
    garis(70);
    printf("%-5s %-*s %14s %6s %8s\n", "Urut", PANJANG_NAMA, "Nama Produk", "Harga", "Stok", "Terjual");
    garis(70);
    for (i = 0; i < jumlahProduk; i++) {
        formatRupiah(harga[urutan[i]], rp);
        printf("%-5d %-*s %14s %6d %8d\n", i + 1, PANJANG_NAMA, nama[urutan[i]],
               rp, stok[urutan[i]], terjual[urutan[i]]);
    }
    garis(70);
}

/* ===================== FITUR 8: LAPORAN PENJUALAN ===================== */

void laporanPenjualan(void)
{
    int i, pertama, totalUnit = 0, maksTerjual = 0;
    long totalPendapatan = 0;
    char rp[32];

    printf("\n=== LAPORAN PENJUALAN ===\n");
    if (jumlahProduk == 0) {
        printf("Belum ada produk.\n");
        return;
    }
    garis(66);
    printf("%-4s %-*s %8s %16s\n", "No", PANJANG_NAMA, "Nama Produk", "Terjual", "Pendapatan");
    garis(66);
    for (i = 0; i < jumlahProduk; i++) {
        formatRupiah(pendapatan[i], rp);
        printf("%-4d %-*s %8d %16s\n", i + 1, PANJANG_NAMA, nama[i], terjual[i], rp);
        totalUnit      += terjual[i];
        totalPendapatan += pendapatan[i];
        if (terjual[i] > maksTerjual) maksTerjual = terjual[i];
    }
    garis(66);
    formatRupiah(totalPendapatan, rp);
    printf("Total unit terjual : %d\n", totalUnit);
    printf("Total pendapatan   : %s\n", rp);

    if (maksTerjual == 0) {
        printf("Produk terlaris    : belum ada penjualan\n");
    } else {
        printf("Produk terlaris    : ");
        pertama = 1;
        for (i = 0; i < jumlahProduk; i++) {
            if (terjual[i] == maksTerjual) {
                printf("%s%s", pertama ? "" : ", ", nama[i]);
                pertama = 0;
            }
        }
        printf(" (%d unit)\n", maksTerjual);
    }
}

/* ===================== FITUR 9: CEK STOK MENIPIS ===================== */

void cekStokMenipis(void)
{
    int i, ada = 0;

    printf("\n=== CEK STOK MENIPIS (stok < %d) ===\n", BATAS_MENIPIS);
    for (i = 0; i < jumlahProduk; i++) {
        if (stok[i] < BATAS_MENIPIS) {
            if (!ada) {
                garis(56);
                printf("%-4s %-*s %14s %6s\n", "No", PANJANG_NAMA, "Nama Produk", "Harga", "Stok");
                garis(56);
            }
            tampilBarisProduk(i);
            ada++;
        }
    }
    if (ada) {
        garis(56);
        printf("[PERINGATAN] %d produk stoknya menipis. Segera lakukan restok!\n", ada);
    } else {
        printf("Semua produk memiliki stok mencukupi.\n");
    }
}

/* ===================== MENU UTAMA ===================== */

void tampilMenu(void)
{
    printf("\n==========================================\n");
    printf("  MONITORING STOK & PENJUALAN KANTIN\n");
    printf("==========================================\n");
    printf(" 1. Lihat Daftar Produk    6. Cari Produk\n");
    printf(" 2. Tambah Produk          7. Urutkan Produk\n");
    printf(" 3. Ubah Produk            8. Laporan Penjualan\n");
    printf(" 4. Transaksi Penjualan    9. Cek Stok Menipis\n");
    printf(" 5. Tambah Stok            0. Keluar\n");
    printf("------------------------------------------\n");
}

int main(void)
{
    int pilihan;

    inisialisasiData();

    do {
        tampilMenu();
        if (!bacaInt("Pilih menu : ", &pilihan)) {
            pilihan = -1;   /* input tidak valid, ulangi menu */
            continue;
        }
        switch (pilihan) {
            case 1: tampilDaftarProduk();  break;
            case 2: tambahProduk();        break;
            case 3: ubahProduk();          break;
            case 4: transaksiPenjualan();  break;
            case 5: tambahStok();          break;
            case 6: cariProduk();          break;
            case 7: urutkanProduk();       break;
            case 8: laporanPenjualan();    break;
            case 9: cekStokMenipis();      break;
            case 0: printf("\nTerima kasih. Program selesai.\n"); break;
            default: printf("[!] Pilihan menu tidak tersedia. Pilih angka 0-9.\n");
        }
    } while (pilihan != 0);

    return 0;
}
