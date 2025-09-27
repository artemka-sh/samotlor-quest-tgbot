# QuizQuest Bot

> A Telegram quest/quiz bot built entirely in modern **C++17** with **Qt** & **TgBot**)

![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)

---

## ✨ Features

| Category        | Details                                                                                                      |
| --------------- | ------------------------------------------------------------------------------------------------------------ |
| Gameplay        | Interactive question‑by‑question quest (supports MCQ & free‑form answers), instant validation, final summary |
| Persistence     | PostgreSQL storage of users & answers, auto‑migration on first launch                                        |
| Config‑driven   | All wording/questions in a single **`questions.json`**; secrets in **`config.json`**                         |
| Keyboard UX     | Reply keyboard with answer options, hides automatically after click                                          |
| Fault tolerance | Long‑poll restart loop, graceful handling of unknown updates, reconnect back‑off                             |
| i18n‑ready      | UTF‑8 everywhere; questions file can be any language                                                         |

---

## 🤔 Why another Telegram bot in C++?

* **Performance & footprint** – native binary starts in \~10 ms and uses <20 MB RAM even on a 512 MB VPS.
* **One dependency graph** – same tool‑chain we already use for the rest of the backend (C++, CMake, clang‑tidy).
* **Fun & curiosity** – pushing C++ outside the usual server/embedded bubble.

If you just need a simple bot, Python is fine. This repo is for people who enjoy unusual tech choices.

---

## 🏗️ Tech stack

* **C++17** – core language, RAII design
* **Qt 5/6** (Core & SQL) – JSON parsing, PostgreSQL driver, cross‑platform utils
* **[TgBot](https://github.com/reo7sp/tgbot-cpp)** – Telegram Bot API for C++
* **PostgreSQL ≥ 14(tested)** – persistent storage 
* **CMake ≥ 3.20** – build system


---

## 📂 Project layout

```
├── CMakeLists.txt
├── src/
│   ├── botapplication.hpp / .cpp   # high‑level orchestrator
│   ├── telegram_messenger.*        # thin wrapper around TgBot::Bot
│   ├── database_manager.*          # Qt SQL helpers
│   ├── questions.*                 # loads questions.json
│   └── config.*                    # config data parser
├── res/
│   ├── config.json                 # secrets & DB creds 
│   └── questions.json              # game script
└── tests/
```

---

## 🚀 Quick start

### 1 · Clone & build

```bash
git clone https://github.com/artemka-sh/samotlor-quest-tgbot/ #you must recursive udate git submodules
cd samotlor-quest-tgbot
cmake ..
cmake --build .
```

### 2 · Create `res/config.json`

```json
{
  "telegram_token": "123456:ABC‑DEF…",
  "db_address": "localhost",
  "db_port": "5432",
  "db_name": "questbot",
  "db_user": "questbot",
  "db_password": "secret",
  "lazy_time": 3600            #don't using
}
```

### 3 · Run migrations (auto)

First launch will create tables `users` and `user_answers` if they don’t exist.

### 4 · Start the bot

```bash
./build/samotlor-quest-tgbot       # logs to stdout
```

## background working with log to file
```bash
 nohup ./samotlor-quest-tgbot >log4.txt 2>&1 &
```

> The binary restarts itself with a built‑in endless loop that catches any exception and starts over.



## 🧩 Extending the quest

1. Open **`res/questions.json`**.
2. Append a new object: `{ "id": 42, "type": "text", "question": "What is the answer?", "allowCustomAnswer": true }`
3. Restart the bot – no recompilation required.

Details on the schema are in `questions.hpp` comments.

---

## 🙌 Contributing

PRs and issues are welcome!
---

## 📜 License

Distributed under the MIT License. See `LICENSE` for details.

## How it's look like
![exampleimage](https://raw.githubusercontent.com/artemka-sh/files/refs/heads/main/exampletgbotq.jpg)

