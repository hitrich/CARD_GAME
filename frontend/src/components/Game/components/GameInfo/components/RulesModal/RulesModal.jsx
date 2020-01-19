import React, { Component } from 'react';
import Modal from 'react-modal';
// Components
import { Button } from 'components';

Modal.setAppElement('#root');

class RulesModal extends Component {
  constructor() {
    // Inherit constructor
    super();
    // State for showing/hiding the modal
    this.state = {
      modalIsOpen: false,
    }
    // Bind functions
    this.openModal = this.openModal.bind(this);
    this.closeModal = this.closeModal.bind(this);
  }

  openModal() {
    this.setState({ modalIsOpen: true });
  }

  closeModal() {
    this.setState({ modalIsOpen: false });
  }

  render() {
    return (
      <div>
        <Button onClick={ this.openModal } className="small blue">RULES</Button>
        <Modal
          isOpen={ this.state.modalIsOpen }
          onRequestClose={ this.closeModal }
          className={ `ModalContent` }
          overlayClassName="ModalOverlay">
          <div className="RulesModal">
            <div className="title">Rules</div>
            <ul>
              <li>
                <div>Каждый игрок начинает с одинаковым набором карт.</div>
                <div>Карты существ имеют два атрибута:</div>
                <ol>
                  <li>Сила: 1, 2, or 3</li>
                  <li>Элементы: Земля, Огонь, или Вода</li>
                </ol>
                <div>Есть также 2 специальные карты:</div>
                <ol>
                  <li>Карта PACEHM аннулирует все результаты в раунде</li>
                  <li>Карта SILVRA имеет мощность 3, но не имеет элементарного бонуса совместимости.</li>
                </ol>
              </li>
              <li><div>В начале игры колоды перемешиваются, и каждый игрок берет 4 карты. Карты в руках всегда открыты.</div></li>
              <li><div>В каждом раунде игроки выбирают карту. Пики выявляются одновременно.</div>
                <div className="divider top"></div>
                <dl>
                  <dt>Расчет баллов</dt>
                  <dd><p className="calculation1"><span className="power">Сила атаки</span><span className="plus">+</span><span className="bonus">Бонус совместимости стихий</span></p></dd>
                </dl>
                <div className="calculations">
                  <div className="calculation2">
                    <div className="description">
                      <div>Сила атаки</div>
                      <div>Значение атаки, указанное на Карте</div>
                    </div>
                  </div>
                  <div className="calculation3">
                    <div className="description">
                      <div>Бонус совместимости стихий</div>
                      <div>Игрок получит 1 дополнительный счет, когда тип элемента карты игрока сильнее, чем у противника (Огонь> Земля> Вода> Огонь> и т. Д.). Для Специальной Карты 2 (SILVRA) Бонус за совместимость элементов не добавляется.</div>
                    </div>
                  </div>
                  <div className="calculation4">
                    <div className="description">
                      <div>Специальная карта 1 (PACEHM)</div>
                      <div>Эта специальная карта 0 силы аттаки аннулирует все результаты в раунде</div>
                    </div>
                  </div>
                  <div className="calculation5">
                    <div className="description">
                      <div>Специальная карта 2 (SILVRA)</div>
                      <div>Эта специальная карта 3 силы аттаки удаляет бонус совместимости элементов в раунде</div>
                    </div>
                  </div>
                </div>
                <div className="divider bottom"></div>
              </li>
              <li><div>Игрок с более высоким счетом выигрывает раунд. Проигравший игрок теряет HP, равную разнице в очках. Например, если игрок А набрал 3 балла, а игрок Б - 1, игрок Б потеряет 2 HP.</div></li>
              <li>
                <div>Игра Завершиться При:</div>
                <ol>
                  <li>У игрока 0 или меньше здоровья. Этот игрок проигравший.</li>
                  <li>Все карты разыграны. Победителем станет игрок с наибольшим количеством жизней. Если итоги жизни одинаковы, ИИ-игрок выигрывает.</li>
                </ol>
              </li>
            </ul>
          </div>
          <div onClick={ this.closeModal }><i className="ButtonClose">Close</i></div>
        </Modal>
      </div>
    )
  }
}

export default RulesModal;
