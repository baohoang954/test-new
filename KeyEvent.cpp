
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(0);
                    break;
                case SDL_KEYDOWN:
                    cerr << "\nDown: " << event.key.keysym.scancode;
                    break;
                case SDL_KEYUP:
                    cerr << "\nUp: " << event.key.keysym.scancode;
                    break;
                default: cerr << "\n.";
            }
        }
        SDL_Delay(100);
    }
