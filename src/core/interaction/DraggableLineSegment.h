// Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
// Use of this source code is governed by the GNU GPLv3 license that can be found in the LICENSE file.

#ifndef SCANTAILOR_INTERACTION_DRAGGABLELINESEGMENT_H_
#define SCANTAILOR_INTERACTION_DRAGGABLELINESEGMENT_H_

#include <QLineF>
#include <QPointF>
#include <boost/function.hpp>

#include "DraggableObject.h"

class ObjectDragHandler;

class DraggableLineSegment : public DraggableObject {
 public:
  using PositionCallback = boost::function<QLineF()>;

  using MoveRequestCallback = boost::function<void(const QLineF& line, Qt::KeyboardModifiers mask)>;

  DraggableLineSegment();

  void setProximityPriority(int priority) { m_proximityPriority = priority; }

  int proximityPriority() const override;

  Proximity proximity(const QPointF& mousePos) override;

  void dragInitiated(const QPointF& mousePos) override;

  void dragContinuation(const QPointF& mousePos, Qt::KeyboardModifiers mask) override;

  void setPositionCallback(const PositionCallback& callback) { m_positionCallback = callback; }

  void setMoveRequestCallback(const MoveRequestCallback& callback) { m_moveRequestCallback = callback; }

 protected:
  virtual QLineF lineSegmentPosition() const { return m_positionCallback(); }

  virtual void lineSegmentMoveRequest(const QLineF& line, Qt::KeyboardModifiers mask) {
    m_moveRequestCallback(line, mask);
  }

 private:
  PositionCallback m_positionCallback;
  MoveRequestCallback m_moveRequestCallback;
  QPointF m_initialMousePos;
  QLineF m_initialLinePos;
  int m_proximityPriority;
};


#endif  // ifndef SCANTAILOR_INTERACTION_DRAGGABLELINESEGMENT_H_
