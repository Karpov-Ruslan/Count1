#pragma once

#include <vector>
#include <limits>
#include <functional>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <exception>
#include <iostream>

namespace krv {

class Map final : public sf::Drawable {
public:

    class Node : public sf::RectangleShape {
    public:
        enum class Type {
            EMPTY,
            BARRIER,
            START,
            FINISH,
            OPEN,
            CLOSE,
            PATH,
        };

        Node(float i, float j) : sf::RectangleShape(sf::Vector2f(1.0f, 1.0f)), pos(i, j) {
            setPosition(j, i);
            setMode(Type::EMPTY);
            setOutlineThickness(0.05f);
            setOutlineColor(sf::Color::Black);
        }

        void setMode(Type newType) {
            type = newType;
            switch (type) {
                case Type::EMPTY: {
                    setFillColor(sf::Color::Green);
                    break;
                }
                case Type::BARRIER: {
                    setFillColor(sf::Color::Red);
                    break;
                }
                case Type::START: {
                    setFillColor(sf::Color::Cyan);
                    break;
                }
                case Type::FINISH: {
                    setFillColor(sf::Color::Blue);
                    break;
                }
                case Type::OPEN: {
                    setFillColor(sf::Color(180, 180, 180));
                    break;
                }
                case Type::CLOSE: {
                    setFillColor(sf::Color(90, 90, 90));
                    break;
                }
                case Type::PATH: {
                    setFillColor(sf::Color::White);
                    break;
                }
            }
        }

        Type getMode() const {
            return type;
        }

        const sf::Vector2u pos;
    private:
        Type type;
    };

    explicit Map(size_t size, const std::vector<sf::Vector2u>& barriers, const sf::Vector2u& start, const sf::Vector2u& finish) : startNode(start), finishNode(finish), time_duration(5.0f/std::pow(static_cast<float>(size), 2.0f)) {
        for (size_t i = 0; i < size; i++) {
            data.emplace_back();
            for (size_t j = 0; j < size; j++) {
                data[i].emplace_back(i, j);
            }
        }

        for (const auto& it : barriers) {
            data[it.x][it.y].setMode(Node::Type::BARRIER);
        }
        data[start.x][start.y].setMode(Node::Type::START);
        data[finish.x][finish.y].setMode(Node::Type::FINISH);
    }

    void pathFind(const std::function<float(sf::Vector2u, sf::Vector2u)>& h, sf::RenderWindow& window) {
        // init data
        std::multimap<float, nodeInfo> queueue;
        std::vector<std::vector<nodeInfo>> closedInfo;
        closedInfo.resize(data.size());
        std::for_each(closedInfo.begin(), closedInfo.end(), [&](std::vector<nodeInfo>& it){it.resize(data.size());});
        {
            float start_h = h(startNode, finishNode);
            queueue.insert({0.0f + start_h, {&data[startNode.x][startNode.y], nullptr, 0.0f, start_h}});
        }
        // end of init data

        while (true) {
            if (queueue.empty()) {
                data[finishNode.x][finishNode.y].setFillColor(sf::Color::Black);
                data[startNode.x][startNode.y].setFillColor(sf::Color::Black);
                break;
            }

            const nodeInfo& tempNode = queueue.begin()->second;
            queueue.erase(queueue.begin());
            closedInfo[tempNode.node->pos.x][tempNode.node->pos.y] = tempNode;
            nodeInfo& curNode = closedInfo[tempNode.node->pos.x][tempNode.node->pos.y];
            curNode.node->setMode(Node::Type::CLOSE);
            // delete this
            curNode.node->setFillColor(sf::Color::Magenta);

            if (curNode.node->pos == finishNode) {
                std::cout << "FINISH" << std::endl;
                data[startNode.x][startNode.y].setMode(Node::Type::START);
                data[finishNode.x][finishNode.y].setMode(Node::Type::FINISH);

                nodeInfo* pathNode = &curNode;
                while(pathNode != nullptr) {
                    if (pathNode->node->pos != finishNode && pathNode->node->pos != startNode) {
                        pathNode->node->setMode(Node::Type::PATH);
                        pathNode = pathNode->parentNodeInfo;

                        window.clear();
                        window.draw(*this);
                        window.display();
                        sf::sleep(sf::seconds(time_duration));

                        continue;
                    }
                    if (pathNode->node->pos == finishNode) {pathNode = pathNode->parentNodeInfo; continue;}
                    if (pathNode->node->pos == startNode) {
                        return;
                    }
                    else {
                        exit(228);
                    }
                }
            }

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i*j != 0 || (i == 0 && j == 0)) {continue;}

                    try {
                        sf::Vector2u childPos(curNode.node->pos.x + i, curNode.node->pos.y + j);
                        Node& childNode = data.at(childPos.x).at(childPos.y); // std::out_of_range exception

                        // check if childNode in queueue
                        bool in_queueue = false;
                        decltype(queueue)::iterator childNodeItInQueueue = queueue.end();
                        for (auto it = queueue.begin(); it != queueue.end(); it++) {
                            if (it->second.node->pos == childNode.pos) {
                                in_queueue = true;
                                childNodeItInQueueue = it;
                            }
                        }

                        // check if childNode in closedInfo
                        bool in_closedInfo = (closedInfo[childPos.x][childPos.y].node != nullptr);

                        if (!in_closedInfo && childNode.getMode() != Node::Type::BARRIER) {
                            if (in_queueue) {
                                float new_cost = curNode.g + 1.0f + h(childPos, finishNode);
                                if (childNodeItInQueueue->first > new_cost) {
                                    queueue.erase(childNodeItInQueueue);
                                    queueue.insert({curNode.g + 1.0f + h(childPos, finishNode), {&childNode, &curNode, curNode.g + 1.0f, h(childPos, finishNode)}});
                                    childNode.setMode(Node::Type::OPEN);
                                }
                            }
                            else {
                                queueue.insert({curNode.g + 1.0f + h(childPos, finishNode), {&childNode, &curNode, curNode.g + 1.0f, h(childPos, finishNode)}});
                                childNode.setMode(Node::Type::OPEN);
                            }
                        }
                    }
                    catch (...) { // maybe other exceptions
                        continue;
                    }
                }
            }

            window.clear();
            window.draw(*this);
            window.display();
            sf::sleep(sf::seconds(time_duration));
            while(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                sf::sleep(sf::milliseconds(1));
            }
            // delete this
            curNode.node->setMode(Node::Type::CLOSE);
        }

    }

    std::vector<Node>& operator[](size_t x) {
        return data[x];
    }

private:
    struct nodeInfo {
        Node* node = nullptr;
        nodeInfo* parentNodeInfo = nullptr;
        float g = NAN;
        float h = NAN;
        float f = g + h;
    };

    std::vector<std::vector<Node>> data;
    const sf::Vector2u startNode;
    const sf::Vector2u finishNode;
    const float time_duration;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (size_t i = 0, size_i = data.size(); i < size_i; i++) {
            for (size_t j = 0, size_j = data.size(); j < size_j; j++) {
                target.draw(data[i][j]);
            }
        }
    }
};

}