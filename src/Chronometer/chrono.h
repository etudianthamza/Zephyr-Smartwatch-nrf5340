#ifndef CHRONO_H
#define CHRONO_H

void chrono_init(void);
void chrono_start(void);
void chrono_stop(void);
void chrono_reset(void);

// Fonction pour obtenir les valeurs actuelles du chronomètre
void chrono_get_time(int *hours, int *minutes, int *seconds);
/* Fonction pour enregistrer un callback appelé à chaque tick (seconde) */
void chrono_set_tick_callback(void (*callback)(void));
#endif