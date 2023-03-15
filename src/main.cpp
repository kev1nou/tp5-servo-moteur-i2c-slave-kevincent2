#include <mbed.h>

#define ADDRESSE_I2C_PAR_DEFAUT 0x51
#define COMMANDE_ALLUMER_MOTEUR 126
#define COMMANDE_ETEINDRE_MOTEUR 127
#define COMMANDE_LIRE_POSITION_MOTEUR 125

#if !DEVICE_I2CSLAVE
#error [NOT_SUPPORTED] I2C Slave is not supported
#endif

// Utiliser la classe I2CSlave pour créer un objet slave.
// Consulter la documentation pour voir les méthodes disponibles.
I2CSlave slave(I2C_SDA, I2C_SCL);

// LED de debug
DigitalOut debug_led(LED1);

// Sortie PWM pour le moteur
PwmOut moteur(PB_6);

// Les états possibles du moteur
enum etat_moteur {Eteint, Allume};
int8_t position_moteur = -127;

int main() {
    
    printf("\r\n\r\n=== Debut du programme Servo-Moteur-I2C (Peripherique) ===\r\n");

    char read_buffer[10];
    char write_buffer[10] = { '\0' };

    // Initialiser le moteur à l'état OFF
    uint8_t etat_moteur = Eteint;

    const int adresse_i2c_7bits = ADDRESSE_I2C_PAR_DEFAUT;
    const int adresse_i2c_8bits = adresse_i2c_7bits << 1; // Nécessairement pair
    slave.address(adresse_i2c_8bits);

    printf("Le slave repond a l'adresse I2C 0x%02X (sur 7 bits).\r\n", adresse_i2c_7bits);
    printf("Sur l'oscilloscope, le premier octet devrait etre 0x%02X ou 0x%02X.\r\n", adresse_i2c_8bits, adresse_i2c_8bits + 1);

    printf("\r\n");

    while (1) {

        // Attendre une requête du master
        int i2c_receive = slave.receive();

        // Traiter la requête
        switch (i2c_receive) {

            // Si le master envoie une requête de lecture
            case I2CSlave::ReadAddressed:
                // Renvoyer l'état du moteur
                if (etat_moteur == Eteint) {
                    sprintf(write_buffer, "OFF");
                }
                else {
                    sprintf(write_buffer, "%d", position_moteur);
                }
                slave.write(write_buffer, sizeof(write_buffer));  // (utiliser la meme taille pour le buffer du peripherique)
                
                printf("=> Demande état moteur\r\n");
                printf("<= %s\r\n\r\n", write_buffer); 

                // Vider le buffer d'écriture
                for (unsigned int i = 0; i < sizeof(write_buffer); i++) {
                    write_buffer[i] = '\0';
                }
  
                break;

            // Si le master envoie une requête de lecture qui nous est adressée
            case I2CSlave::WriteAddressed:
                slave.read(read_buffer, 10);
                int8_t commande_recue = read_buffer[0];

                printf("=> Commande moteur : %d\r\n\r\n", commande_recue);

                if (commande_recue == COMMANDE_ALLUMER_MOTEUR) {
                    etat_moteur = Allume;
                    moteur.resume();
                    moteur.period_ms(20);
                    moteur.pulsewidth_us(1500);
                }
                else if (commande_recue == COMMANDE_ETEINDRE_MOTEUR) {
                    etat_moteur = Eteint;
                    moteur.suspend();
                }
                else if (commande_recue >= -90 && commande_recue <= 90) {
                    position_moteur = commande_recue;
                    uint16_t pulse_width = 1500 + commande_recue * 6.666;
                    moteur.period_ms(20);
                    moteur.pulsewidth_us(pulse_width);
                }
                else {
                    printf("=  Commande invalide !\r\n");
                }
                break;    
        }
        
        // Vider le buffer de lecture
        for (int i = 0; i < sizeof(read_buffer); i++) {
            read_buffer[i] = 0;
        }
        
        // Vider le buffer d'écriture
        for (int i = 0; i < sizeof(write_buffer); i++) {
            write_buffer[i] = 0;
        }
    }
}