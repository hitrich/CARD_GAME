#include "cardgame.hpp"
#include <eosio/system.hpp>

// Простой алгоритм псевдослучайных чисел, случайным образом выбрать число в диапазоне от 0 до n-1
int cardgame::random(const int range) {
  // Найти существующее начальное значение
  auto seed_iterator = _seed.begin();

  // Инициализировать начальное значение со значением по умолчанию, если оно не найдено
  if (seed_iterator == _seed.end()) {
    seed_iterator = _seed.emplace( _self, [&]( auto& seed ) { });
  }

  // Создать новое начальное значение, используя существующее начальное значение
  int prime = 65537;
  auto new_seed_value = (seed_iterator->value + current_time_point().elapsed.count()) % prime;
  
  // Сохранение значения обновления в таблице
  _seed.modify( seed_iterator, _self, [&]( auto& s ) {
    s.value = new_seed_value;
  });
  
  // Получить случайный результат в нужном диапазоне
  int random_result = new_seed_value % range;
  return random_result;
}

// Взять одну карту из колоды и назначьте ее на руку
void cardgame::draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand) {
  // Выбор случайной карты из колоды
  int deck_card_idx = random(deck.size());

  // Поиск первой пустой ячейки в руке
  int first_empty_slot = -1;
  for (int i = 0; i <= hand.size(); i++) {
    auto id = hand[i];
    if (card_dict.at(id).type == EMPTY) {
      first_empty_slot = i;
      break;
    }
  }
  check(first_empty_slot != -1, "No empty slot in the player's hand");

  // Присвоение карты первому пустому слоту в руке
  hand[first_empty_slot] = deck[deck_card_idx];
  
  // Удалить карту из колоды
  deck.erase(deck.begin() + deck_card_idx);
}

// Рассчет последней точки атаки карты после учета элементального бонуса
int cardgame::calculate_attack_point(const card& card1, const card& card2) {
  int result = card1.attack_point;

  //Добавить элементальный бонус совместимости 1
  if ((card1.type == FIRE && card2.type == WOOD) ||
      (card1.type == WOOD && card2.type == WATER) ||
      (card1.type == WATER && card2.type == FIRE)) {
    result++;
  }

  return result;
}

// AI Лучшая стратегия выигрыша карты
int cardgame::ai_best_card_win_strategy(const int ai_attack_point, const int player_attack_point) {
  eosio::print("Best Card Wins");
  if (ai_attack_point > player_attack_point) return 3;
  if (ai_attack_point < player_attack_point) return -2;
  return -1;
}

// AI Стратегия минимизации потерь
int cardgame::ai_min_loss_strategy(const int ai_attack_point, const int player_attack_point) {
  eosio::print("Minimum Losses");
  if (ai_attack_point > player_attack_point) return 1;
  if (ai_attack_point < player_attack_point) return -4;
  return -1;
}

// AI Стратегия подсчета очков
int cardgame::ai_points_tally_strategy(const int ai_attack_point, const int player_attack_point) {
  eosio::print("Points Tally");
  return ai_attack_point - player_attack_point;
}

// AI Стратегия предотвращения потерь
int cardgame::ai_loss_prevention_strategy(const int8_t life_ai, const int ai_attack_point, const int player_attack_point) {
  eosio::print("Loss Prevention");
  if (life_ai + ai_attack_point - player_attack_point > 0) return 1;
  return 0;
}

// Рассчет для текущей карты с учетом стратегии и карт игрока
int cardgame::calculate_ai_card_score(const int strategy_idx, 
                                      const int8_t life_ai,
                                      const card& ai_card, 
                                      const vector<uint8_t> hand_player) {
   int card_score = 0;
   for (int i = 0; i < hand_player.size(); i++) {
      const auto player_card_id = hand_player[i];
      const auto player_card = card_dict.at(player_card_id);

      int ai_attack_point = calculate_attack_point(ai_card, player_card);
      int player_attack_point = calculate_attack_point(player_card, ai_card);

      // Накопление счета карты на основе данной стратегии
      switch (strategy_idx) {
        case 0: {
          card_score += ai_best_card_win_strategy(ai_attack_point, player_attack_point);
          break;
        }
        case 1: {
          card_score += ai_min_loss_strategy(ai_attack_point, player_attack_point);
          break;
        }
        case 2: {
          card_score += ai_points_tally_strategy(ai_attack_point, player_attack_point);
          break;
        }
        default: {
          card_score += ai_loss_prevention_strategy(life_ai, ai_attack_point, player_attack_point);
          break;
        }
      }
    }
    return card_score;
}

// Выбор карты из руки ИИ на основе текущих данных игры
int cardgame::ai_choose_card(const game& game_data) {
  // Четвертая стратегия выбрана только в тяжелой ситуации
  int available_strategies = 4; 
  if (game_data.life_ai > 2) available_strategies--; 
  int strategy_idx = random(available_strategies);
 
  // Рассчет каждой карты в руке AI
  int chosen_card_idx = -1;
  int chosen_card_score = std::numeric_limits<int>::min(); 

  for (int i = 0; i < game_data.hand_ai.size(); i++) {
    const auto ai_card_id = game_data.hand_ai[i];
    const auto ai_card = card_dict.at(ai_card_id);

    // Игнорировать пустой слот в руке
    if (ai_card.type == EMPTY) continue;

    // Рассчет для этой карты AI относительно карт игрока
    auto card_score = calculate_ai_card_score(strategy_idx, game_data.life_ai, ai_card, game_data.hand_player);

    // Наблюдение за картой, которая имеет самый высокий балл
    if (card_score > chosen_card_score) {
      chosen_card_score = card_score;
      chosen_card_idx = i;
    }
  }
  return chosen_card_idx;
}

// Устраните выбранные карты и обновить урон
void cardgame::resolve_selected_cards(game& game_data) {
  const auto player_card = card_dict.at(game_data.selected_card_player);
  const auto ai_card = card_dict.at(game_data.selected_card_ai);

  // Для типа VOID мы пропустим любой расчет ущерба
  if (player_card.type == VOID || ai_card.type == VOID) return;

  int player_attack_point = calculate_attack_point(player_card, ai_card);
  int ai_attack_point =  calculate_attack_point(ai_card, player_card);

  // Расчет ущерба
  if (player_attack_point > ai_attack_point) {
    // Наносит урон ИИ, если точка атаки ИИ карты выше
    int diff = player_attack_point - ai_attack_point;
    game_data.life_lost_ai = diff;
    game_data.life_ai -= diff;
  } else if (ai_attack_point > player_attack_point) {
    // Нанести урон игроку, если точка атаки карты игрока выше
    int diff = ai_attack_point - player_attack_point;
    game_data.life_lost_player = diff;
    game_data.life_player -= diff;
  }
}

// Проверка текущей игровой доски и обновление статуса игры соответственно
void cardgame::update_game_status(user_info& user) {
  game& game_data = user.game_data;

  if (game_data.life_ai <= 0) {
    // Проверка  AI на HP
    game_data.status = PLAYER_WON;
  } else if (game_data.life_player <= 0) {
    // Проверка игрока на HP
    game_data.status = PLAYER_LOST;
  } else {
    // Ни у одного из игроков HP не уменьшено до 0
    // Проверка, закончилась ли игра (то есть больше нет карт в обеих руках)
    const auto is_empty_slot = [&](const auto& id) { return card_dict.at(id).type == EMPTY; };
    bool player_finished = std::all_of(game_data.hand_player.begin(), game_data.hand_player.end(), is_empty_slot);
    bool ai_finished = std::all_of(game_data.hand_ai.begin(), game_data.hand_ai.end(), is_empty_slot);

    // Если у одного из них закончилась карта, то и у другого тоже
    if (player_finished || ai_finished) {
      if (game_data.life_player > game_data.life_ai) {
        game_data.status = PLAYER_WON;
      } else {
        game_data.status = PLAYER_LOST;
      }
    }
  }

  // Обновите счет проигранных / выигранных соответственно
  if (game_data.status == PLAYER_WON) {
    user.win_count++;
  } else if (game_data.status == PLAYER_LOST) {
    user.lost_count++;
  } 
}
