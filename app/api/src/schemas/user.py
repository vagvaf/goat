from typing import List, Optional

from pydantic import BaseModel, EmailStr

from src.db import models
from src.resources.enums import LanguageEnum


class UserBase(models.UserBase):
    roles: List[str] = []
    study_areas: List[int] = []


class UserCreate(UserBase):
    password: str


class UserUpdate(UserBase):
    name: Optional[str] = None
    surname: Optional[str] = None
    email: EmailStr = None
    password: Optional[str] = None
    organization_id: Optional[int] = None
    roles: Optional[List[str]] = None
    study_areas: Optional[List[int]] = None


class UserStudyAreaList(BaseModel):
    id: int
    name: str


class UserPreference(BaseModel):
    language_preference: Optional[LanguageEnum]
    active_study_area_id: Optional[int]
    
    class Config:
        extra = "forbid"


"""
Body of the request
"""
request_examples = {
    "create": {
        "name": "John",
        "surname": "Doe",
        "email": "john.doe@email.com",
        "password": "secret",
        "roles": ["superuser", "user"],
        "study_areas": [1],
        "active_study_area_id": 1,
        "organization_id": 1,
        "active_data_upload_ids": [],
        "is_active": True,
        "storage": 512000,
        "limit_scenarios": 50,
    },
    "update": {
        "name": "Kevin",
        "surname": "Cross",
        "email": "kevin.cross@email.com",
        "password": "updated_secret",
        "roles": ["user"],
        "study_areas": [],
        "active_study_area_id": 1,
        "organization_id": 1,
        "is_active": False,
        "storage": 1500000,
        "limit_scenarios": 10,
    },
    "update_user_preference": {
        "language_preference": {
            "summary": "Update language preference",
            "value": {
                "language_preference": "en",
            }
        },
        "study_area_preference": {
            "summary": "Update study area preference",
            "value": {         
                "active_study_area_id": 1,
            }
        }
        ,
        "language_study_area_preference": {
            "summary": "Both language and study area preferences",
            "value": {         
                "language_preference": "en",
                "active_study_area_id": 1,
            }
        }
    },
}
