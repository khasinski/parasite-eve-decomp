void sndTrackReadAdsrDecayRate(void *ptr);
void sndTrackReadAdsrSustainLevel(void *ptr, int arg);

void SeqOp_ReadAdsrDecayAndSustain(void *ptr, int arg) {
    sndTrackReadAdsrDecayRate(ptr);
    sndTrackReadAdsrSustainLevel(ptr, arg);
}
