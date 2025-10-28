// Bloqueo básico de anuncios por clase e ID
document.addEventListener("DOMContentLoaded", () => {
  const selectors = [
    ".ad", ".ads", "#ad", "#ads", ".banner", ".sponsored", "[id^='ad-']"
  ];
  selectors.forEach(sel => {
    document.querySelectorAll(sel).forEach(el => el.remove());
  });
});
