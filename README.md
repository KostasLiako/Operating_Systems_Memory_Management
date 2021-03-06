# ΕΡΓΑΣΙΑ 2 Λειτουργικά Συστήματα.

## Εντολές Μεταγλώττισης - Εκτέλεσης:
**Clone the repo** 
```sh
    git clone https://github.com/KostasLiako/Operating_Systems_Memory_Management.git
```
**Compilation**
```sh
    make
```

**Program execution** 
```sh
    ./main <algorithm> <frames> <q>
```
 Algorithm = {“LRU”,”SC”}
<br>
<br>
### Εκτέλεση: 
Κατά την εκτέλεση το πρόγραμμα μας διαβάζει εναλλάξ σύνολα αναφορών (πλήθους q) από τα δύο επιμέρους αρχεία αναφορών τα οποία υπάρχουν στον φάκελο Trace_files. Συνολικά διαβάζουμε αριθμό αναφορών MAX_TRACES που έχει οριστεί ως define στο main πρόγραμμα.
<br>
<br>
### Υλοποίηση: 
Για την υλοποίηση της εργασίας θα χρησιμοποιήσουμε δύο Hash Page Tables όπου θα αποθηκεύονται η σελίδες των επιμέρους διεργασιών και μία δομή ουράς όπου θα προσομοιώνει την κύρια μνήμη. Δημιουργούμε τις δομές του Hash Page Table, της σελίδας (Page) και της ουράς (quequeMemory) και της αρχικοποιούμε κατάλληλα. Στην συνέχεια ξεκινάει η διαδικασία της προσομοίωσης. Η προσομοίωση υλοποιείται στην συνάρτηση simulation().
 Αρχικά υπολογίζουμε τον αριθμό της σελίδας κάνοντας δεξιά ολίσθηση 12 bits από την κάθε αναφορά. Κάθε σελίδα έχει και ένα αναγνωριστικό R ή W αν αποτελεί εγγραφή η ανάγνωση. Τέλος σε κάθε σελίδα τις δίνεται ένας αριθμός counter.
<br>
<br>
### Εκχώρηση Σελίδας: 
Αρχικά η συνάρτηση της προσομοίωσης ελέγχει αν η εκάστοτε σελίδα υπάρχει στην μνήμη. Αν δεν υπάρχει τότε μέσω της συνάντησης hashFunc() υπολογίζουμε το hash code της έτσι ώστε να την εκχωρήσουμε στην σωστή θέση του ανάλογου Hash Page Table(HPT). Οι σελίδες στο ίδιο bucket συνδέονται μεταξύ τους μέσω μίας απλής συνδεδεμένης λίστας. Αφού γίνει η παραπάνω διαδικασία εκχωρείται στον HPT και στην ουρά όπου έχουμε υλοποιήσει. Στα επιμέρους HPT μπορόυν να εκχωρηθούν συνολικά μεχρί frames σελίδες. Ο έλεγχος αν υπάρχει διαθέσιμο frame γίνεται πριν την εκχώρηση κάθε σελίδας.Τέλος αυξάνεται ο αριθμός των Reads ή Writes ανάλογα με το αναγνωριστικό που έχει η σελίδα που εκχωρείται.
<br>
<br>
### Σελίδα που ήδη υπάρχει: 
Αν προκύψει αναφορά για μία σελίδα που ήδη υπάρχει στην μνήμη (δηλαδή στην ουρά) τότε ανάλογα με τον αλγόριθμο που έχουμε δώσει στην γραμμή εντολών (“LRU” ή “SC”) εκτελούμε την ανάλογη διαδικασία. Στην περίπτωση που ο αλγόριθμος είναι ο LRU βρίσκουμε την σελίδα στην ουρά και την μετακινούμε στο τέλος της. Στην
περίπτωση που ο αλγόριθμος είναι ο SC (Second Chance) βρίσκουμε την σελίδα μέσα στην ουρά και αντικαθιστούμε το Reference Bit απο 0 σε 1. Στην περίπτωση που είναι 1 τότε δεν το αλλάζουμε. Τέλος αυξάνεται ο αριθμός των Reads ή Writes ανάλογα με το αναγνωριστικό που έχει η σελίδα στο αντίστοιχο HPT.
<br>
<br>
### Μη διαθέσιμος χώρος για σελίδα: 
Στην περίπτωση που δεν υπάρχει διαθέσιμο frame για την εκχώρηση μίας σελίδας που δεν υπάρχει στην μνήμη πρέπει να οριστεί μία σελίδα, που υπάρχει ήδη στην μνήμη, ως victim και να αντικατασταθεί από την νέα σελίδα. Η παραπάνω διαδικασία υλοποιείται στην συνάρτηση findVictim(). Ανάλογα με τον αλγόριθμο που έχουμε δώσει, υλοποιούνται η παρακάτω διαδικασίες.
Η σελίδα που διαγράφεται από την μνήμη ελέγχουμε αν είναι Εγγραφής η Ανάγνωσης. Αν είναι εγγραφής αυξάνουμε τον μετρητή Writes on Disc.
<br>
<br>
### LRU: 
Στην περίπτωση που ο αλγόριθμος είναι ο LRU τότε ως victim ορίζεται η κεφαλή της ουράς καθώς είναι η τελευταία χρησιμοποιούμενη σελίδα. Οπότε διαγράφουμε την κεφαλή και στο τέλος της ουράς βάζουμε την καινούργια σελίδα το οποίο γίνεται στην συνάρτηση replaceVictim().
<br>
<br>
### Second Chance (SC): 
Στην περίπτωση που ο αλγόριθμος είναι ο SC τότε για να βρούμε την σελίδα victim ξεκινάμε την προσπέλαση απο την αρχή της
ουράς και την πρώτη σελίδα, αν το Reference bit της είναι ίσο με 0 τοτε αυτή είναι η σελίδα victim και στην θέση της βάζουμε την καινούργια σελίδα. (Δεν την μετακινούμε την νέα σελίδα στο τέλος της ουράς). Στην περίπτωση που το Reference Bit είναι ίσο με 1 τότε του εκχωρούμε την τιμή 0 και προχωράμε στην επόμενη σείδα στην ουρά μέχρι να βρούμε την σελίδα με το Reference Bit == 0. Αν φτάσουμε στο τέλος της ουράς και δεν έχουμε βρει την σελίδα victim τότε ξεκινάμε πάλι απο την αρχή της.
<br>
<br>
<br>
Τέλος εκτυπώνονται τα στατιστικά κάθε πίνακα. Αριθμός Εγγραφών , Αναγνώσεων ,σελίδες που γράφτηκαν πίσω στον δίσκο, Page Faults (Σφάλματα Σελίδας) και οι επιμέρους Hash Page Tables.
