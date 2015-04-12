//
//  scene.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/16/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/scene.h>

using namespace std;

namespace rp { namespace ui {

    Scene::Scene(const string& name) : backgroundColor_(colors::black), name_(name) {}
    Scene::~Scene() {}
    
    string Scene::getName() const {
        return name_;
    }
    
    void Scene::setName(const std::string &newName) {
        name_ = newName;
    }
    
    Color Scene::getBackgroundColor() const {
        return backgroundColor_;
    }
    
    void Scene::setBackgroundColor(rp::ui::Color c) {
        backgroundColor_ = c;
    }
    
    void Scene::apply(std::shared_ptr<IDisplayObjectVisitor> visitor) {
        visitor->visit(dynamic_pointer_cast<Scene>(shared_from_this()));
    }
    
}}

